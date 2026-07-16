#include "MouseHandler.hpp"


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


void MouseHandler::mouseCallback(
    int event,
    int x,
    int y,
    int flags,
    void* userdata
)
{
    if (event != cv::EVENT_LBUTTONDOWN)
        return;


    auto* handler =
        static_cast<MouseHandler*>(userdata);


    auto position =
        handler->translator_.pixelToCell(x, y);


    // לחיצה מחוץ ללוח
    if (!position.has_value())
        return;


    if (handler->callback_)
        handler->callback_(position.value());
}