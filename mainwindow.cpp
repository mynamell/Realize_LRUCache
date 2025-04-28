#include "mainwindow.h"
#include "ui_mainwindow.h"  // 注意包含正确的头文件

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::LRUCache),  // 改为LRUCache
    cache(new LRUCache(50, 200, "image_cache", this))
{
    ui->setupUi(this);

    connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::loadImage);
    connect(ui->clearButton, &QPushButton::clicked, cache, &LRUCache::clear);
    connect(cache, &LRUCache::imageReady, this, &MainWindow::showImage);
    connect(cache, &LRUCache::loadError, this, &MainWindow::showError);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadImage()
{
    QString url = ui->urlEdit->text();
    if(url.isEmpty()) return;

    // 简单验证URL格式
    if(!url.startsWith("http")) {
        ui->statusLabel->setText("URL must start with http:// or https://");
        return;
    }
    cache->getImage(url);
}

void MainWindow::showImage(const QPixmap &image, const QString &url)
{
    ui->imageLabel->setPixmap(image.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->statusLabel->setText("Loaded: " + url);
}

void MainWindow::showError(const QString &url, const QString &error)
{
    ui->statusLabel->setText("Error loading " + url + ": " + error);
}
