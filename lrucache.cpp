#include "lrucache.h"
#include <QCryptographicHash>// 包含 QCryptographicHash，用于生成文件名的哈希值
#include <QNetworkRequest>// 包含 QNetworkRequest，用于设置网络请求
#include <QStandardPaths>
//构造
LRUCache::LRUCache(int memorySize,int fileSize,const QString &cacheDir,QObject* parent)
    :QObject(parent),m_memorySize(memorySize),m_fileSize(fileSize),m_cacheDir(cacheDir)
{
    QDir dir;//创建QDir对象，用于目录操作
    if(!dir.exists(m_cacheDir)){//检查缓存目录是否存在
        dir.mkpath(m_cacheDir);//不存在则创建缓存目录
    }
    m_networkManager=new QNetworkAccessManager(this);//创建网络访问管理器
    connect(m_networkManager,&QNetworkAccessManager::finished,this,&LRUCache::onImageDownloaded);//当网络请求结束调用槽函数方法

}
//析构
LRUCache::~LRUCache(){
    clear();//清空缓存
    delete m_networkManager;//删除网络访问管理器对象
}




//网络请求完成槽函数执行方法
void LRUCache::onImageDownloaded(QNetworkReply* reply){
    QString url=reply->request().attribute(QNetworkRequest::User).toString();//获取请求url
    if(reply->error()!=QNetworkReply::NoError){
        //如果请求出错
        emit loadError(url,reply->errorString());//发出加载错误信号
        reply->deleteLater();//删除回复对象
        return;//返回，不执行后续代码
    }
    QByteArray imageData=reply->readAll();//读取回复数据
    reply->deleteLater();//删除回复对象
    QPixmap image;//创建QPixmap对象
    if(!image.loadFromData(imageData)){//如果无法从数据加载图片
        emit loadError(url,"Invalid image data");//发出加载错误信号
        return;
    }
    QString key=QString::number(qHash(url));//生成url对应的值为键
    QMutexLocker locker(&m_mutex);
    addToMemory(key,image);
    addToFile(key,imageData);
    emit imageReady(image,url);//发出图像准备就绪信号
}
//获取图片方法
void LRUCache::getImage(const QString &url){
    QMutexLocker locker(&m_mutex);//add互斥锁
    // QString key=QString::number(qHash(url));//url的哈希值作为键
    // 改用更可靠的哈希方式
    QString key = QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5).toHex();
    //1.检查内存缓存
    if(m_memoryCache.contains(key)){//如果内存缓存中存在键值
        promote(key);
        emit imageReady(m_memoryCache[key].image,url);//发出图像准备就绪信号
        return;
    }

    //从网络下载
    QNetworkRequest request(url);//创建网络请求
    request.setAttribute(QNetworkRequest::User,url);//设置请求属性
    m_networkManager->get(request);//发送get请求
}
//清空缓存方法
void LRUCache::clear(){
    QMutexLocker locker(&m_mutex);
    m_memoryCache.clear();//清空内存缓存
    m_memoryKeys.clear();//清空内存缓存键列表

}
//将图片增加到内存缓存方法
void LRUCache::addToMemory(const QString &key,const QPixmap &image){
    ensureMemorySpace();//确保内存缓存有足够的空间
    CacheItem item;//创建缓存项
    item.image=image;//设置缓存项图片
    m_memoryCache.insert(key,item);//将缓存项插入内存缓存
    m_memoryKeys.prepend(key);//将键增加到内存缓存键列表开头
}
//确认内存缓存是否有足够空间
void LRUCache::ensureMemorySpace(){
    while(m_memoryKeys.size()>=m_memorySize){//内存缓存满了进行remove
        QString oldestKey=m_memoryKeys.takeLast();//获取最旧的键
        m_memoryCache.remove(oldestKey);//从内存缓存中移除该键
    }
}
//获取路径url
QString LRUCache::cacheFilePath(const QString &key) const{
    return m_cacheDir+"/"+key+".cache";//返回缓存文件路径
}
