#pragma once

void (* ProcessSysMessages)();
void (* PollInput)();
void (* RenderScreen)();

namespace NCEngine{


    void Initialize(void (* message)(), void (* input)(), void (* render)()){
        ProcessSysMessages = message;
        PollInput = input;
        RenderScreen = render;
    }

    void MainLoop(){

        bool isRunning = true;
        // while (isRunning){

        //     (*ProcessSysMessages)();
        //     (*PollInput)();
        //     (*RenderScreen)();
        // }


    }

}
