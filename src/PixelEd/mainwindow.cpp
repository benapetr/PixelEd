//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2024

#include <PixelEngine/engine.h>
#include <PixelEngine/world.h>
#include <PixelEngine/ringlog.h>
#include <PixelEngine/ringlog_item.h>
#ifdef PIXELED_GL
#include <PixelEngine/Graphics/peglwidget.h>
#include <PixelEngine/Graphics/qglrenderer.h>
#else
#include <PixelEngine/Graphics/qimagerenderer.h>
#endif
#include <QKeyEvent>
#include <QDateTime>
#include <QImage>
#include <QLayout>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    PE::Engine::Initialize(false);
    ui->setupUi(this);
    this->showMaximized();
#ifndef PIXELED_GL
    this->fps = 0;
    this->fps_current = 0;
    this->fps_start = QDateTime::currentDateTime().toMSecsSinceEpoch();
    this->se_renderer = new PE::QImageRenderer(this->GetWidth(), this->GetHeight());
    this->se_renderer->ManualUpdate = true;
    this->qimage = this->se_renderer->GetImage();
#else
    this->viewPort = new PE::PEGLWidget(this, nullptr);
    this->layout()->removeWidget(this->ui->viewPort);
    delete this->ui->viewPort;
    this->ui->viewPort = nullptr;
    this->layout()->addWidget(this->viewPort);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::GetWidth()
{
#ifndef PIXELED_GL
    return this->ui->viewPort->width();// * QApplication::desktop()->devicePixelRatio();
#else
    return this->viewPort->width();
#endif
}

int MainWindow::GetHeight()
{
#ifndef PIXELED_GL
    return this->ui->viewPort->height();// * QApplication::desktop()->devicePixelRatio();
#else
    return this->viewPort->height();
#endif
}

double MainWindow::GetFPS()
{
#ifdef PIXELED_GL
   return this->viewPort->GetFPS();
#else
    return this->fps;
#endif
}

void MainWindow::InstallWorld(PE::World *w)
{
#ifdef PIXELED_GL
    this->viewPort->SetWorld(w);
#endif
}

void MainWindow::UninstallWorld()
{
#ifdef PIXELED_GL
    this->viewPort->SetWorld(nullptr);
#endif
}
