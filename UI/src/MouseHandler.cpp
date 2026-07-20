#include "MouseHandler.hpp"
#include <iostream>

void MouseHandler::attachTo(const std::string& windowName)
{
    cv::setMouseCallback(
        windowName,
        MouseHandler::mouseCallback,
        this
    );
}


void MouseHandler::setOnClick(ClickCallback callback)
{
    callback_ = callback;
}

void MouseHandler::setOnRightClick(ClickCallback callback)
{
    rightClickCallback_ = callback;
}

void MouseHandler::mouseCallback( int event,  int x,    int y,  int flags, void* userdata)
{
      std::cout << "mouseCallback event=" << event << std::endl;
    if (event != cv::EVENT_LBUTTONDOWN && event != cv::EVENT_RBUTTONDOWN)
        return;

    auto* handler =
        static_cast<MouseHandler*>(userdata);

    auto position =
        handler->translator_.pixelToCell(x, y);

    // לחיצה מחוץ ללוח
    if (!position.has_value())
        return;

    if (event == cv::EVENT_LBUTTONDOWN)
    {
        if (handler->callback_)
            handler->callback_(position.value());
    }
    else if (event == cv::EVENT_RBUTTONDOWN)
    {
        if (handler->rightClickCallback_)
            handler->rightClickCallback_(position.value());
    }
}