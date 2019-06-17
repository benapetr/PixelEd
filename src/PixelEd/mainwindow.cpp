//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QKeyEvent>
#include <QDateTime>
#include <QImage>
#include <QDesktopWidget>
#include <PixelEngine/engine.h>
#include <PixelEngine/world.h>
#include <PixelEngine/ringlog.h>
#include <PixelEngine/ringlog_item.h>
#include <PixelEngine/Graphics/peglwidget.h>
#include <PixelEngine/Graphics/qglrenderer.h>
#include "editor.h"

MainWindow *MainWindow::Main = nullptr;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    Main = this;
    PE::Engine::Initialize(false);
    this->ui->setupUi(this);
    this->showMaximized();
    this->viewPort = new PE::PEGLWidget(this, nullptr);
    this->layout()->addWidget(this->viewPort);
    this->renderTimer = new QTimer(this);
    this->editor = new Editor();
    connect(this->renderTimer, SIGNAL(timeout()), this, SLOT(OnRender()));
    // this timer speed defines FPS, smaller means higher FPS, but also more CPU usage
    this->renderTimer->start(20);
}

MainWindow::~MainWindow()
{
    delete this->viewPort;
    delete this->editor;
    delete this->ui;
}

int MainWindow::GetWidth()
{
    return this->viewPort->width();
}

int MainWindow::GetHeight()
{
    return this->viewPort->height();
}

double MainWindow::GetFPS()
{
   return this->viewPort->GetFPS();
}

void MainWindow::InstallWorld(PE::World *w)
{
    this->viewPort->SetWorld(w);
}

void MainWindow::UninstallWorld()
{
    this->viewPort->SetWorld(nullptr);
}

void MainWindow::OnRender()
{
    this->viewPort->update();
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    //this->game->GetWorld()->ProcessKeyPress(e->key());
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    //this->game->GetWorld()->ProcessKeyRelease(e->key());
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit();
}
