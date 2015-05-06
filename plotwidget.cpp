#include "plotwidget.hpp"
#include "ui_plotwidget.h"

void PlotWidget::paintLoop()
{
    while(true)
    {
        QMetaObject::invokeMethod(this, "repaint", Qt::QueuedConnection);
        while(!m_canUpdate.load());
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void PlotWidget::paintEvent(QPaintEvent *)
{
    m_canUpdate.store(false);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(243, 255, 155));
    painter.drawRect(0, 0, this->width() - painter.pen().width(), this->height() - painter.pen().width());
    QPen pen;
    pen.setBrush(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);

    painter.drawLine(QPoint(xMargin, 0), QPoint(xMargin, this->height()));
    painter.drawLine(QPoint(0, this->height() - yMargin), QPoint(this->width(), this->height() - yMargin));
    painter.drawText(QPoint(5, 10), "Fehler");
    painter.drawText(QPoint(this->width() - 70, this->height() - 10), "Ausführungen");
    if(m_dots.size() > 0)
        painter.drawText(QPoint(5, this->height() - (yMargin + m_dots.at(0) * m_biggestScale)), QString::number(m_dots.at(0)));
    pen.setBrush(Qt::red);
    painter.setPen(pen);
    painter.setBrush(Qt::red);
    double xScale = (double)(this->width() - xMargin) /(double) m_dots.size();
    for(int i = 1; i < m_dots.size(); i++)
    {
        painter.drawLine(QPoint(xMargin + xScale * (i - 1), this->height() - (yMargin + m_dots.at(i - 1) * m_biggestScale)), QPoint(xMargin + xScale * i, this->height() - (yMargin + m_dots.at(i) * m_biggestScale)));
    }
    m_canUpdate.store(true);
}

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::PlotWidget),
      m_dots(),
      m_biggestScale(0)
{
    ui->setupUi(this);
    m_canUpdate.store(true);
    m_paintingThread = new std::thread(&PlotWidget::paintLoop, this);
}

PlotWidget::~PlotWidget()
{
    delete ui;
}

void PlotWidget::addDot(double p_dot)
{
    double curScale = (double)(this->height() - yMargin * 2) / p_dot;
    if(curScale < m_biggestScale || m_biggestScale == 0)
        m_biggestScale = curScale;
    m_dots.push_back(p_dot);
}

void PlotWidget::clearDots()
{
    m_biggestScale = 0;
    m_dots.clear();
}
