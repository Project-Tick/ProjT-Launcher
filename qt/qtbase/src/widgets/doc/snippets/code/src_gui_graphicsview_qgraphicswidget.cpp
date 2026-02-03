// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGraphicsWidget>
#include <QStyleOptionGroupBox>

class MyGroupBoxWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    void initStyleOption(QStyleOption *option) const;
    void examples(QGraphicsWidget *a, QGraphicsWidget *b,
                  QGraphicsWidget *c, QGraphicsWidget *d);
    bool isFlat() const;
};

//! [0]
void MyGroupBoxWidget::initStyleOption(QStyleOption *option) const
{
    QGraphicsWidget::initStyleOption(option);
    if (QStyleOptionGroupBox *box = qstyleoption_cast<QStyleOptionGroupBox *>(option)) {
        // Add group box specific state.
        box->text = tr("My Group Box");
        //...
    }
}
//! [0]


void MyGroupBoxWidget::examples(QGraphicsWidget *a, QGraphicsWidget *b,
                                QGraphicsWidget *c, QGraphicsWidget *d)
{
    {
        //! [1]
        setTabOrder(a, b); // a to b
        setTabOrder(b, c); // a to b to c
        setTabOrder(c, d); // a to b to c to d
        //! [1]
    }

    {
        //! [2]
        // WRONG
        setTabOrder(c, d); // c to d
        setTabOrder(a, b); // a to b AND c to d
        setTabOrder(b, c); // a to b to c, but not c to d
        //! [2]
    }
}
