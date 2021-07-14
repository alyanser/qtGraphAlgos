#ifndef SCENE_HPP
#define SCENE_HPP

#include <QGraphicsScene>
#include <queue>
#include <stack>

class QTabWidget;
class QSize;
class QLineEdit;
class QGraphicsGridLayout;
class Node;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QTimer;
class QPushButton;
class StackedWidget;
class QLabel;

class GraphicsScene : public QGraphicsScene{
         Q_OBJECT
         Q_PROPERTY(bool running WRITE setRunning READ isRunning);
         using pIntNode = std::pair<int,Node*>;
public:
         explicit GraphicsScene(QSize size);
         ~GraphicsScene();
         GraphicsScene(const GraphicsScene & other) = delete;
         GraphicsScene(GraphicsScene && other) = delete;
         GraphicsScene & operator = (const GraphicsScene & other) = delete;
         GraphicsScene & operator = (GraphicsScene && other) = delete;
private:
         uint32_t timerDelay; 
         const QSize windowSize;
         QGraphicsScene * innerScene;
         std::unique_ptr<StackedWidget> helpDialogWidget;
         std::unique_ptr<QTabWidget> bar;
         QTimer * bfsTimer,* dfsTimer,* dijkstraTimer,* pathTimer; 
         std::pair<size_t,size_t> sourceNodeCord;
         std::pair<size_t,size_t> targetNodeCord;
         QGraphicsGridLayout * innerLayout; 
         std::unique_ptr<std::queue<std::pair<Node*,int>>> queue; 
         std::unique_ptr<std::stack<std::pair<Node*,int>>> stack;
         std::unique_ptr<std::vector<std::vector<bool>>> visited; 
         std::unique_ptr<std::vector<std::vector<int>>> distance;
         std::unique_ptr<std::priority_queue<pIntNode,std::vector<pIntNode>,std::greater<>>> pq;
         bool running = false; 
         Node * sourceNode = nullptr; 
         Node * targetNode = nullptr; 
         constexpr inline static int yOffset = -135; // px : 50 topbar 50 bottombar 35 padding
         constexpr inline static int rowCnt = 10;
         constexpr inline static int colCnt = 20;
         constexpr inline static uint32_t defaultDelay = 100; // ms
         constexpr inline static std::array<int,4> xCord {-1,1,0,0};
         constexpr inline static std::array<int,4> yCord {0,0,1,-1};

         void populateBar();
         void populateWidget(QWidget * widget,const QString & algoName,const QString & infoText);
         void populateGridScene(); 
         void populateLegend(QWidget * parentWidget,QVBoxLayout * sideLayout) const;
         void populateBottomLayout(QWidget * parentWidget,QGridLayout * mainLayout) const;
         void populateSideLayout(QWidget * parent,QVBoxLayout * sideLayout,const QString & algoName,const QString & infoText);

         void configureMachine(QWidget * parentWidget,QPushButton * statusButton);

         void allocTimers();
         void setMainSceneConnections() const;
         void connectPaths() const;
         void configureInnerScene();
         void generateRandGridPattern();
         void allocDataStructures();
         void setRunning(bool newState); 
         bool isRunning() const; 
         void cleanup() const; 
         void resetGrid() const;
         void updateSourceTargetNodes() const;
         static bool validCordinate(int row,int col);
         Node * getNewNode(size_t row,size_t col);
         Node * getNodeAt(size_t row,size_t col) const;
         QLineEdit * getStatusBar(size_t tabIndex) const;
         bool isBlock(Node * currentNode) const;
         bool isSpecial(Node * currentNode) const;
         void setTimersIntervals(uint32_t newSpeed) const;
         void memsetDs() const;
         void stopTimers() const;
         void pathConnect() const; 
         static std::pair<int,int> getRandomCord();
         static void addShadowEffect(QLabel * label);
         QHBoxLayout * getLegendLayout(QWidget * parentWidget,QString token) const;
         void disableBarTabs(int exception) const;
         void enableAllBarTabs() const;
         
         void bfsConnect() const;
         void dfsConnect() const;
         void dijkstraConnect() const;
         
         void bfsStart(bool newStart) const;
         void dfsStart(bool newStart) const;
         void dijkstraStart(bool newStart) const;
public slots:
         void setDelay(uint32_t newDelay);
signals:
         void foundPath() const; 
         void close() const;
         void resetButtons() const; 
         void runningStatusChanged(bool newState) const; 
         void animationDurationChanged(uint32_t newDuration) const; 
};

#endif
