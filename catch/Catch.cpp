#include "Catch.h"
#include "ui_Catch.h"
#include "Player.h"

#include <QDebug>
#include <QMessageBox>
#include <QActionGroup>
#include <QSignalMapper>

Catch::Catch(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::Catch),
      m_player(Player::player(Player::Red)) {

    ui->setupUi(this);

    QObject::connect(ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(reset()));
    QObject::connect(ui->actionQuit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(showAbout()));

    QSignalMapper* map = new QSignalMapper(this);
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QString cellName = QString("cell%1%2").arg(row).arg(col);
            Cell* cell = this->findChild<Cell*>(cellName);
            Q_ASSERT(cell != nullptr);
            Q_ASSERT(cell->row() == row && cell->col() == col);

            m_board[row][col] = cell;

            int id = row * 8 + col;
            map->setMapping(cell, id);
            QObject::connect(cell, SIGNAL(clicked(bool)), map, SLOT(map()));
            QObject::connect(cell, SIGNAL(mouseOver(bool)), this, SLOT(updateSelectables(bool)));
        }
    }
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    QObject::connect(map, SIGNAL(mapped(int)), this, SLOT(play(int)));
#else
    QObject::connect(map, SIGNAL(mappedInt(int)), this, SLOT(play(int)));
#endif

    // When the turn ends, switch the player.
    QObject::connect(this, SIGNAL(turnEnded()), this, SLOT(switchPlayer()));
    QObject::connect(this, SIGNAL(fim()), this, SLOT(fimDeJogo()));

    this->reset();

    this->adjustSize();
    this->setFixedSize(this->size());
}

Catch::~Catch() {
    delete ui;
}

void Catch::play(int id) {
    Cell* cell = m_board[id / 8][id % 8];
    if (cell == nullptr || !cell->isSelectable())
        return;
    Cell*cell2;
    if(m_player->orientation() == Player::Horizontal){
        cell2 = m_board[id / 8][(id % 8)+1];
        if((id % 8)+1 >7 || cell2->isBlocked()){
            //cell->setState(Cell::Empty);
        }else{
            cell->setState(Cell::Blocked);
            cell2->setState(Cell::Blocked);
            this->fazClusters();
            emit turnEnded();
            //this->fazClusters();
        }
    }else{
        cell2 = m_board[(id/8)+1][id % 8];
        if((id / 8)+1 >7 || cell2->isBlocked()){

        }else{
            cell->setState(Cell::Blocked);
            cell2->setState(Cell::Blocked);
            this->fazClusters();
            emit turnEnded();
            //this->fazClusters();
        }
    }
}

void Catch::fazClusters() {
    QList<Cell*> todos;
    QList<Cell*> workList;
    QList<Cell*> cluster;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Cell* cell = m_board[row][col];
            if(cell->isEmpty()){
                todos.append(cell);
            }
        }
    }
    while (todos.length()>0) {
            Cell* atual;
            atual = todos.first();
            cluster.append(atual);
            todos.removeFirst();
            Cell* cell1;
            Cell* cell2;
            Cell* cell3;
            Cell* cell4;
            if((atual->row()-1)>=0){
                cell1 = m_board[atual->row()-1][atual->col()];
                if(cell1!=nullptr){
                if(todos.contains(cell1)){
                    workList.append(cell1);
                    todos.removeOne(cell1);
                }
                }
            }
            if((atual->col()+1)<=7){
                cell2 = m_board[atual->row()][atual->col()+1];
                if(cell2!=nullptr){
                if(todos.contains(cell2)){
                    workList.append(cell2);
                    todos.removeOne(cell2);
                }
                }
            }
            if((atual->row()+1)<=7){
                cell3 = m_board[atual->row()+1][atual->col()];
                if(cell3!=nullptr){
                if(todos.contains(cell3)){
                    workList.append(cell3);
                    todos.removeOne(cell3);
                }
                }
            }
            if((atual->col()-1)>=0){
                cell4 = m_board[atual->row()][atual->col()-1];
                if(cell4!=nullptr){
                if(todos.contains(cell4)){
                    workList.append(cell4);
                    todos.removeOne(cell4);
                }
                }
            }

            while (workList.length()>0) {
                atual = workList.first();
                cluster.append(atual);
                workList.removeFirst();
                if((atual->row()-1)>=0){
                    cell1 = m_board[atual->row()-1][atual->col()];
                    if(cell1!=nullptr){
                    if(todos.contains(cell1)){
                        workList.append(cell1);
                        todos.removeOne(cell1);
                    }
                    }
                }
                if((atual->col()+1)<=7){
                    cell2 = m_board[atual->row()][atual->col()+1];
                    if(cell2!=nullptr){
                    if(todos.contains(cell2)){
                        workList.append(cell2);
                        todos.removeOne(cell2);
                    }
                    }
                }
                if((atual->row()+1)<=7){
                    cell3 = m_board[atual->row()+1][atual->col()];
                    if(cell3!=nullptr){
                    if(todos.contains(cell3)){
                        workList.append(cell3);
                        todos.removeOne(cell3);
                    }
                    }
                }
                if((atual->col()-1)>=0){
                    cell4 = m_board[atual->row()][atual->col()-1];
                    if(cell4!=nullptr){
                    if(todos.contains(cell4)){
                        workList.append(cell4);
                        todos.removeOne(cell4);
                    }
                    }
                }
                //cluster.append(atual);
            }
            if(cluster.length()>3){
                cluster.clear();
            }//While que faz os clusters corretos pontuarem
            while(cluster.length()<=3 && cluster.length()>0){
                Cell* aux= cluster.first();
                cluster.removeFirst();
                //emit aux->stateChanged();
                //aux->setState(Cell::Captured);
                aux->setPlayer(m_player);
                m_player->incrementCount();
                //aux->update();
            }



    }
}
void Catch::switchPlayer() {
    // Switch the player.
    m_player = m_player->other();
    this->jogadaImpossivel();
}

void Catch::reset() {
    // Reset board.
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Cell* cell = m_board[row][col];
            cell->reset();
        }
    }

    // Reset the players.
    Player* red = Player::player(Player::Red);
    red->reset();

    Player* blue = Player::player(Player::Blue);
    blue->reset();

    m_player = red;

    // Finally, update the status bar.
    this->updateStatusBar();
}

void Catch::showAbout() {
    QMessageBox::information(this, tr("Sobre"), tr("Catch\n\nAndré Cordeiro - andrecordeiro0931@gmail.com\nSamuel Pedro - samuelpedrofernandesamorim@gmail.com "));
}

void Catch::updateSelectables(bool over) {
    Cell* cell = qobject_cast<Cell*>(QObject::sender());
    Q_ASSERT(cell != nullptr);
    Cell* cell2;
    Q_ASSERT(cell != nullptr);
    if(m_player->orientation()== Player::Horizontal){
        if(cell->col()<7){
            cell2 = m_board[cell->row()][cell->col()+1];
            if (over) {
                if (cell->isEmpty() && cell2->isEmpty()) {
                    cell->setState(Cell::Selectable);
                    cell2->setState(Cell::Selectable);
                }

            } else {
                if (cell->isSelectable() && cell2->isSelectable()) {
                    cell->setState(Cell::Empty);
                    cell2->setState(Cell::Empty);
                }

            }
        }

    }else{
        if(cell->row()<7){
            cell2 = m_board[cell->row()+1][cell->col()];
            if (over) {
                if (cell->isEmpty() && cell2->isEmpty()) {
                    cell->setState(Cell::Selectable);
                    cell2->setState(Cell::Selectable);
                }
            } else {
                if (cell->isSelectable() && cell2->isSelectable()) {
                    cell->setState(Cell::Empty);
                    cell2->setState(Cell::Empty);
                }
            }
        }
    }
}

void Catch::updateStatusBar() {
    ui->statusbar->showMessage(tr("Vez do %1 (%2 a %3)")
        .arg(m_player->name()).arg(m_player->count()).arg(m_player->other()->count()));
}

void Catch::jogadaImpossivel(){
    Cell*cell;
    Cell*cell2;
    int sinal =1;
    if(m_player->orientation() == Player::Horizontal){
        for(int i=0; i<8; i++){
            for(int j=0; j<7; j++){
                cell= m_board[i][j];
                cell2=m_board[i][j+1];
                if(cell->isEmpty()&&cell2->isEmpty()){
                    sinal =0;
                }
            }
        }
        this->updateStatusBar();
    }else{
        for(int i=0; i<7; i++){
            for(int j=0; j<8; j++){
                cell= m_board[i][j];
                cell2=m_board[i+1][j];
                if(cell->isEmpty()&&cell2->isEmpty()){
                    sinal =0;
                }
            }
        }
        this->updateStatusBar();
    }
    if(sinal==1){
        emit fim();
    }

}

void Catch::fimDeJogo(){

    Player*player1 = m_player;
    Player*player2 = m_player->other();

    if(player1->count() > player2->count()){
        QMessageBox::information(this, tr("Vencedor"), tr("Parabéns, o %1 venceu de %2 a %3.").arg(player1->name()).arg(player1->count()).arg(player2->count()));
    }else if(player2->count() > player1->count()){
        QMessageBox::information(this, tr("Vencedor"), tr("Parabéns, o %1 venceu de %2 a %3.").arg(player2->name()).arg(player2->count()).arg(player1->count()));
    }else{
        QMessageBox::information(this, tr("Empate"), tr("O jogo empatou em %1 a %2.").arg(player1->count()).arg(player2->count()));
    }

}
