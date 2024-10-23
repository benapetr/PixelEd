//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU Lesser General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU Lesser General Public License for more details.

// Copyright (c) Petr Bena 2024

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

class QImage;

namespace PE
{
    class PEGLWidget;
    class QImageRenderer;
    class World;
}

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        void Render();
        int GetWidth();
        int GetHeight();
        double GetFPS();
        void InstallWorld(PE::World *w);
        void UninstallWorld();

    private:
#ifdef PIXELED_GL
        PE::PEGLWidget *viewPort;
#endif
        QTimer *renderTimer;
        QImage *qimage;
#ifndef PIXELED_GL
        double fps;
        int fps_current;
        qint64 fps_start;
        double real_fps = 0;
        int rc_fps = 0;
        PE::QImageRenderer *se_renderer;
#endif
        Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
