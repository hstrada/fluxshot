#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QMouseEvent>
#include <QClipboard>
#include <QGuiApplication>
#include <QWidget>
#include <QKeyEvent>

class ScreenshotWidget : public QWidget {
public:
    ScreenshotWidget() {
        screen = QGuiApplication::primaryScreen();
        fullScreenshot = screen->grabWindow(0);

        setWindowFlags(Qt::FramelessWindowHint);
        showFullScreen();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);

        painter.drawPixmap(0, 0, fullScreenshot);

        // overlay escuro
        painter.fillRect(rect(), QColor(0, 0, 0, 120));

        if (selecting) {
            QRect selection = QRect(startPoint, endPoint).normalized();

            // área limpa
            painter.drawPixmap(selection, fullScreenshot, selection);

            painter.setPen(QPen(Qt::red, 2));
            painter.drawRect(selection);
        }
    }

    void mousePressEvent(QMouseEvent *event) override {
        selecting = true;
        startPoint = event->position().toPoint();
        endPoint = startPoint;
        update();
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        endPoint = event->position().toPoint();
        update();
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        selecting = false;
        endPoint = event->position().toPoint();

        QRect selectedRect = QRect(startPoint, endPoint).normalized();
        QPixmap cropped = fullScreenshot.copy(selectedRect);

        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setPixmap(cropped, QClipboard::Clipboard);

        // esconde a janela (mas mantém o app vivo!)
        this->hide();
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Escape) {
            QApplication::quit();
        }
    }

private:
    QScreen *screen;
    QPixmap fullScreenshot;

    bool selecting = false;
    QPoint startPoint, endPoint;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ScreenshotWidget w;
    w.show();

    return app.exec();
}