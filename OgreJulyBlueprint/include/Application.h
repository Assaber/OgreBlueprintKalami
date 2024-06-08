#pragma once
#include <QApplication>

class Application : public QApplication
{
    using super = QApplication;
    using super::super;

public:
    int join(std::function<void(void)>&& loop);

protected:
    virtual bool event(QEvent* e) override;

private:
    bool mbEnable = true;
};