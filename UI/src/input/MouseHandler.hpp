#pragma once

#include <opencv2/opencv.hpp>
#include <functional>
#include "ClickTranslator.hpp"
#include "Position.hpp"
#include "model/Board.hpp"


class MouseHandler {
public:

    using ClickCallback =
        std::function<void(const Position&)>;

    using VoidCallback =
        std::function<void()>; 
   
    explicit MouseHandler(ClickTranslator& translator)
        : translator_(translator) {}


    void attachTo(const std::string& windowName);


    void setOnClick(ClickCallback callback);
    void setOnRightClick(ClickCallback callback);
    void setOnOutsideClick(VoidCallback callback); 

private:

    static void mouseCallback(
        int event,
        int x,
        int y,
        int flags,
        void* userdata
    );


    ClickTranslator& translator_;

    ClickCallback callback_;
    ClickCallback rightClickCallback_;
     VoidCallback outsideClickCallback_;

};