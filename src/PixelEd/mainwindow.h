//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2019

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui
{
    class MainWindow;
}

class QImage;

namespace PE
{
    class PEGLWidget;
    class QImageRenderer;
    class World;
}

class Editor;

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        static MainWindow *Main;

        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override;
        int GetWidth();
        int GetHeight();
        double GetFPS();
        void InstallWorld(PE::World *w);
        void UninstallWorld();

    public slots:
        void OnRender();
        void keyPressEvent(QKeyEvent* e) override;
        void keyReleaseEvent(QKeyEvent* e) override;

    private slots:
        void on_actionExit_triggered();

    private:
        Editor *editor;
        PE::PEGLWidget *viewPort;
        QTimer *renderTimer;
        Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
