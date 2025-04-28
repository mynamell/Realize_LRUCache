#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "lrucache.h"
namespace Ui {
class LRUCache;  // 注意这里改为LRUCache
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadImage();
    void showImage(const QPixmap &image, const QString &url);
    void showError(const QString &url, const QString &error);

private:
     Ui::LRUCache *ui;
    LRUCache *cache;
};

#endif // MAINWINDOW_H
