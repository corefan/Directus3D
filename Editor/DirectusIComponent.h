/*
Copyright(c) 2016 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//=================================
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include "Core/GameObject.h"
#include "DirectusDropDownButton.h"
//=================================

class DirectusInspector;

class DirectusIComponent : public QWidget
{
    Q_OBJECT
public:
    explicit DirectusIComponent(QWidget* parent = 0) : QWidget(parent){}

    virtual void Initialize(DirectusInspector* inspector, QWidget* mainWindow) = 0;
    virtual void Reflect(std::weak_ptr<Directus::GameObject> gameObject) = 0;

protected:
    //= TITLE ==============================
    QLabel* m_title;
    DirectusDropDownButton* m_optionsButton;
    //======================================

    //= LINE =======
    QWidget* m_line;
    //==============

    //= MISC ========================================
    QGridLayout* m_gridLayout;
    DirectusInspector* m_inspector;
    std::weak_ptr<Directus::GameObject> m_gameObject;
    //===============================================

signals:
    //void IStorageSignal1(const QString &par1);

public slots:
    virtual void Remove() = 0;
};
