#include <QGraphicsScene>
#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsView>
#include <QPainter>
#include <QTimeLine>
#include "node.hpp"

Node::Node(int row,int col,QGraphicsItem * parent) : QGraphicsObject(parent), currentLocation{row,col}, 
         backwardTimer(new QTimeLine()), forwardTimer(new QTimeLine()){

         setAcceptDrops(true);
         setAcceptHoverEvents(true);
         setGraphicsItem(this);
         configureBackwardTimer();
         configureForwardTimer();

         bool startTimer = false;
         setType(type,startTimer);
}

Node::~Node(){
         delete backwardTimer;
         delete forwardTimer;
}

void Node::setRunningState(const bool newState){
         algoRunning = newState;
}

QRectF Node::boundingRect() const{
         return QRectF(0,0,dimension,dimension);
}

void Node::paint(QPainter * painter,const QStyleOptionGraphicsItem * option,QWidget * widget){
         Q_UNUSED(option);
         Q_UNUSED(widget);

         if(type == Visited){
                  painter->setOpacity(opacity() / 2.0);
         }
         painter->setRenderHint(QPainter::Antialiasing);
         painter->drawPixmap(0,0,dimension,dimension,icon);
}

QSizeF Node::sizeHint(Qt::SizeHint which, const QSizeF & constraint) const{
         Q_UNUSED(which);
         Q_UNUSED(constraint);

         return QSizeF(dimension,dimension);
}

void Node::setGeometry(const QRectF & geometry){
         prepareGeometryChange();
         QGraphicsLayoutItem::setGeometry(geometry);
         setPos(geometry.topLeft());
}

void Node::configureBackwardTimer(){
         backwardTimer->setDirection(QTimeLine::Backward);
         backwardTimer->setFrameRange(70,100);
         backwardTimer->setDuration(backwardDuration);

         connect(backwardTimer,&QTimeLine::frameChanged,[this](int delta){
                  const qreal converted = delta / 100.0;
                  setOpacity(converted);
                  setScale(converted);
                  update();
         });

         connect(backwardTimer,&QTimeLine::finished,[forwardTimer = forwardTimer]{
                  forwardTimer->start();
         });
}

void Node::configureForwardTimer(){
         forwardTimer->setDirection(QTimeLine::Forward);
         forwardTimer->setFrameRange(70,100);
         forwardTimer->setDuration(forwardDuration);

         connect(forwardTimer,&QTimeLine::frameChanged,[this](int delta){
                  const qreal converted = delta / 100.0;
                  setOpacity(converted);
                  setScale(converted);
                  update();
         });
}

void Node::setType(const State newType,const bool startTimer){
         type = newType;
         
         bool acceptDrag = true;

         if(type == Source){
                  acceptDrag = false;
                  emit sourceSet(); // update source node in GraphicsScene class
         }else if(type == Target){
                  acceptDrag = false;
                  emit targetSet(); // update target node in GraphicsScene class
         }
         setAcceptDrops(acceptDrag); 

         switch(type){
                  case Source : icon.load(":/pixmaps/icons/source.png");break;
                  case Target : icon.load(":/pixmaps/icons/target.png");break;
                  case Active : icon.load(":/pixmaps/icons/active.png");break;
                  case Inactive : icon.load(":/pixmaps/icons/inactive.png");break;
                  case Visited : icon.load(":/pixmaps/icons/inactive.png");break;
                  case Block : icon.load(":/pixmaps/icons/block.png");break;
                  case Inpath : icon.load(":/pixmaps/icons/inpath.png");break;
                  default : assert(false);
         }
         if(startTimer && backwardTimer->state() == QTimeLine::NotRunning){
                  backwardTimer->start();
         }else{
                  update();
         }
}

void Node::setPathParent(Node * newParent){
         pathParent = newParent;
}

std::pair<int,int> Node::getCord() const{
         return currentLocation;
}

Node * Node::getPathParent() const{
         return pathParent;
}

Node::State Node::getType() const{
         return type;
}

void Node::dragEnterEvent(QGraphicsSceneDragDropEvent * event){
         auto mimeData = event->mimeData();

         if(mimeData->hasText()){
                  const QString & text = mimeData->text();
                  if(text == "inverter"){
                           switch(type){
                                    case Source | Target : break;
                                    case Block : setType(Inactive);event->accept();break;
                                    default : setType(Block);event->accept();
                           }
                  }
         }
         QGraphicsObject::dragEnterEvent(event);
}

void Node::dropEvent(QGraphicsSceneDragDropEvent * event){
         auto mimeData = event->mimeData();
         if(mimeData->hasText()){
                  const QString & indicator = mimeData->text();

                  if(indicator == "fromSource"){
                           setType(Source);
                           event->accept();
                  }else if(indicator == "fromTarget"){
                           setType(Target);
                           event->accept();
                  }
         }
         QGraphicsItem::dropEvent(event);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent * event){
         auto dragger = new QDrag(this);
         auto mimeData = new QMimeData();
         dragger->setMimeData(mimeData);
         
         switch(type){
                  case Source : {
                           dragger->setPixmap(icon);
                           mimeData->setText("fromSource");
                           if(dragger->exec()){
                                    setType(Inactive);
                           }
                           break;
                  }
                  case Target : {
                           dragger->setPixmap(icon);
                           mimeData->setText("fromTarget");
                           if(dragger->exec()){
                                    setType(Inactive);
                           }
                           break;
                  }
                  default  : {
                           mimeData->setText("inverter");
                           dragger->exec();
                  }
         }
         QGraphicsItem::mousePressEvent(event);
}