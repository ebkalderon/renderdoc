#include "ThumbnailStrip.h"
#include <QDebug>
#include <QScrollBar>
#include "Widgets/ResourcePreview.h"
#include "ui_ThumbnailStrip.h"

ThumbnailStrip::ThumbnailStrip(QWidget *parent) : QWidget(parent), ui(new Ui::ThumbnailStrip)
{
  ui->setupUi(this);

  layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
  layout->setSpacing(6);
  layout->setContentsMargins(6, 6, 6, 6);
  layout->setAlignment(Qt::AlignTop);
}

ThumbnailStrip::~ThumbnailStrip()
{
  delete layout;
  delete ui;
}

void ThumbnailStrip::AddPreview(ResourcePreview *prev)
{
  layout->addWidget(prev);
  m_Thumbnails.push_back(prev);
}

void ThumbnailStrip::ClearThumbnails()
{
  for(ResourcePreview *p : m_Thumbnails)
  {
    layout->removeWidget(p);
    delete p;
  }

  m_Thumbnails.clear();
}

void ThumbnailStrip::resizeEvent(QResizeEvent *event)
{
  RefreshLayout();
}

void ThumbnailStrip::mousePressEvent(QMouseEvent *event)
{
  emit(mouseClick(event));
}

void ThumbnailStrip::RefreshLayout()
{
  QRect avail = geometry();
  avail.adjust(6, 6, -6, -6);

  int numActive = 0;
  for(ResourcePreview *c : m_Thumbnails)
    if(c->isActive())
      numActive++;

  // depending on overall aspect ratio, we either lay out the strip horizontally or
  // vertically. This tries to account for whether the strip is docked along one side
  // or another of the texture viewer
  if(avail.width() > avail.height())
  {
    avail.setWidth(avail.width() + 6);    // controls implicitly have a 6 margin on the right

    int aspectWidth = (int)((float)avail.height() * 1.3f);

    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    delete layout;
    layout = new QHBoxLayout(ui->scrollAreaWidgetContents);
    for(ResourcePreview *w : m_Thumbnails)
      layout->addWidget(w);
    layout->setSpacing(6);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setAlignment(Qt::AlignTop);

    int noscrollWidth = numActive * (aspectWidth + 20);

    if(noscrollWidth <= avail.width())
    {
      ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

      for(ResourcePreview *c : m_Thumbnails)
        c->SetSize(QSize(aspectWidth, avail.height()));
    }
    else
    {
      ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

      QScrollBar *hs = ui->scrollArea->horizontalScrollBar();

      avail.setHeight(avail.height() - hs->geometry().height());

      aspectWidth = (int)((float)avail.height() * 1.3f);

      int totalWidth = numActive * (aspectWidth + 20);
      hs->setEnabled(totalWidth > avail.width());

      for(ResourcePreview *c : m_Thumbnails)
        c->SetSize(QSize(aspectWidth, avail.height()));
    }
  }
  else
  {
    avail.setHeight(avail.height() + 6);    // controls implicitly have a 6 margin on the bottom

    int aspectHeight = (int)((float)avail.width() / 1.3f);

    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    delete layout;
    layout = new QVBoxLayout(ui->scrollAreaWidgetContents);
    for(ResourcePreview *w : m_Thumbnails)
      layout->addWidget(w);
    layout->setSpacing(6);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setAlignment(Qt::AlignTop);

    int noscrollHeight = numActive * (aspectHeight + 6);

    if(noscrollHeight <= avail.height())
    {
      ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

      for(ResourcePreview *c : m_Thumbnails)
        c->SetSize(QSize(avail.width(), aspectHeight));
    }
    else
    {
      ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

      QScrollBar *vs = ui->scrollArea->verticalScrollBar();

      avail.setWidth(avail.width() - vs->geometry().width());

      aspectHeight = (int)((float)avail.width() / 1.3f);

      int totalHeight = numActive * (aspectHeight + 6);
      vs->setEnabled(totalHeight > avail.height());

      for(ResourcePreview *c : m_Thumbnails)
        c->SetSize(QSize(avail.width(), aspectHeight));
    }
  }
}