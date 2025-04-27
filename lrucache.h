#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <QObject>  // 包含 Qt 的基类 QObject，用于支持信号和槽机制
#include <QHash>    // 包含 QHash，用于快速查找缓存项
#include <QList>    // 包含 QList，用于实现 LRU 策略
#include <QString>  // 包含 QString，用于处理字符串
#include <QByteArray>  // 包含 QByteArray，用于处理二进制数据
#include <QFile>   // 包含 QFile，用于文件操作
#include <QDir>    // 包含 QDir，用于目录操作
#include <QMutex>  // 包含 QMutex，用于线程安全
#include <QNetworkAccessManager>  // 包含 QNetworkAccessManager，用于网络请求
#include <QPixmap> // 包含 QPixmap，用于处理图片
#include <QNetworkReply>

QT_BEGIN_NAMESPACE

class LRUCache : public QObject
{
    Q_OBJECT

public:
    LRUCache(int memorySize=50,int fileSize=200,const QString &cacheDir=QString(),QObject *parent = nullptr);
    ~LRUCache();

    // 公共方法
    void getImage(const QString &url);
    void clear();

signals:
    void imageReady(const QPixmap &image,const QString &url);
    void loadError(const QString &url,const QString &error);
private slots:
    void onImageDownloaded(QNetworkReply *reply);
private:
    struct CacheItem//定义结构体，存储缓存项
    {
        QPixmap image;//存储图像数据
    };
    void addToMemory(const QString &key,const QPixmap &image);//将图像增加至内存缓存
    void addToFile(const QString &key,const QByteArray &imageData);//将图片数据增加至内存缓存
    void ensureMemorySpace();//确保内存缓存有足够空间
    void ensureFileSpace();//确保文件缓存有足够空间
    void promote(const QString& key);//提升某个缓存项的优先级
    QString cacheFilePath(const QString &key) const;//获取缓存文件的路径

    int m_memorySize;//内存缓存大小
    int m_fileSize;//文件缓存大小
    QString m_cacheDir;//缓存目录

    QHash<QString,CacheItem> m_memoryCache;//内存缓存的哈希表
    QList<QString> m_memoryKeys;//内存缓存的键列表,用于实现LRU策略
    QHash<QString,bool> m_fileCache;//文件缓存的哈希表
    QList<QString> m_fileKeys;//文件缓存的键列表
    QNetworkAccessManager *m_networkManager;//网络访问管理器，用于处理网络请求
    mutable QMutex m_mutex;//互斥锁，保证线程安全

};
#endif // LRUCACHE_H
