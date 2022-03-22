#define SDL_MAIN_HANDLED
#include <vector>
#include "SDL_main.h"
#include "SDL.h"
#include <SDL_gamecontroller.h>
#include <iostream>
#include <cmath>
#include <WS2tcpip.h>
#include <windows.h>
#include <shellapi.h>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;



int main(int argc, char* args[])
{
    // error Check for UDP send
    int sendOk = -1;
    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    SDL_Surface* gXOut = NULL;

    //The window we'll be rendering to
    SDL_Window* window = NULL;

    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

    SDL_GameController* controller = NULL;

    // Data variable to write information about setup
    WSADATA data;
    // Word that has version of winsock. In this case 2.2
    WORD version = MAKEWORD(2, 2);
    int wsOk;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }



    //Create window
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    //Get window surface
    screenSurface = SDL_GetWindowSurface(window);

    //Fill the surface white
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

    //Update the surface
    SDL_UpdateWindowSurface(window);

    //Launch google meet
    std::cout << "Opening google meet!\n";
    //ShellExecute(0, 0, L"https://meet.google.com/dsm-viwn-xir", 0, 0, SW_SHOW);

    //Wait 2 seconds
    SDL_Delay(2000);

    // Startup winsock and write info to variable
    wsOk = WSAStartup(version, &data);
    if (wsOk != 0) {
        cout << "Can't start Winsock!\n";
        return 1;
    }

    //Create socket
    SOCKET out = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in client;
    client.sin_addr.S_un.S_addr = ADDR_ANY;
    client.sin_family = AF_INET;
    client.sin_port = htons(54000);

    // sockaddr_in is used for ipv4
    sockaddr_in server;
    // send to the following address
    inet_pton(AF_INET, "192.168.194.239", &server.sin_addr);
    // use ipv4 family
    server.sin_family = AF_INET;
    // set port to 54000
    server.sin_port = htons(4444); // Convert from little to big endian

    //Bind Socket to the address and port being used
    if (bind(out, (sockaddr*)&client, sizeof(client)) == SOCKET_ERROR) {
        cout << "Can't bind socket! " << WSAGetLastError() << endl;
        return 1;
    }
    char buf[5];
    buf[0] = 1;
    SDL_GameControllerEventState(SDL_ENABLE);
    //While application is running
    while (!quit)
    {
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type) {
            case SDL_KEYDOWN:
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_CONTROLLERDEVICEADDED:
                // open the controller to the game controller instance to manipulate it later
                controller = SDL_GameControllerOpen(e.cdevice.which);
                //print out its name to the console
                cout << "Controller name: " << SDL_GameControllerNameForIndex(e.cdevice.which) << "\n";
                //if there was an error opening the controller, print out the error
                if (!SDL_GameControllerGetAttached(controller)) {
                    cout << "SDL_GetError() = " << SDL_GetError() << "\n";
                }
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                buf[1] = 0b00000010;
                buf[2] = 0;

                buf[3] = 0b00000010;
                buf[4] = 0;
                cout << "CONTROLLER REMOVED\n";
                sendOk = sendto(out, buf, 5, 0, (sockaddr*)&server, sizeof(server));
                if (sendOk == SOCKET_ERROR) {
                    cout << "ERROR: " << WSAGetLastError() << "\n";
                }
                break;
            case SDL_CONTROLLERBUTTONUP:
                buf[1] = 0b00000010;
                buf[2] = 0;

                buf[3] = 0b00000010;
                buf[4] = 0;
                sendOk = sendto(out, buf, 5, 0, (sockaddr*)&server, sizeof(server));
                if (sendOk == SOCKET_ERROR) {
                    cout << "ERROR: " << WSAGetLastError() << "\n";
                }
                break;
            case SDL_CONTROLLERBUTTONDOWN:

                if (e.cbutton.button == 11) {
                    buf[1] = 0b00000100;
                    buf[2] = 75;

                    buf[3] = 0b00000000;
                    buf[4] = 75;
                }
                else if (e.cbutton.button == 12) {
                    buf[1] = 0b00000000;
                    buf[2] = 25;

                    buf[3] = 0b00000001;
                    buf[4] = 25;
                }
                else if (e.cbutton.button == 13) {
                    buf[1] = 0b00000100;
                    buf[2] = 75;

                    buf[3] = 0b00000001;
                    buf[4] = 75;
                }
                else if (e.cbutton.button == 14) {
                    buf[1] = 0;
                    buf[2] = 75;

                    buf[3] = 0;
                    buf[4] = 75;
                }
                sendOk = sendto(out, buf, 5, 0, (sockaddr*)&server, sizeof(server));
                if (sendOk == SOCKET_ERROR) {
                    cout << "ERROR: " << WSAGetLastError() << "\n";
                }
                cout << "Button " << unsigned(e.cbutton.button) << " Pressed\n";
                break;
            case SDL_CONTROLLERAXISMOTION:
                buf[1] = 0b00001000;
                buf[2] = 0;

                buf[3] = 0b00000010;
                buf[4] = 0;

                cout << "Axis " << unsigned(e.caxis.axis) << " moved(" << e.caxis.value << ")\n";
                break;
            }

        }
        //Apply the image
        SDL_BlitSurface(gXOut, NULL, screenSurface, NULL);

        //Update the surface
        SDL_UpdateWindowSurface(window);
    }
    ZeroMemory(buf, 5);

    sendOk = sendto(out, buf, 5, 0, (sockaddr*)&server, sizeof(server));
    if (sendOk == SOCKET_ERROR) {
        cout << "ERROR: " << WSAGetLastError() << "\n";
    }

    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}


/*
#define SDL_MAIN_HANDLED
#include <vector>
#include "SDL_main.h"
#include "SDL.h"
#include <SDL_gamecontroller.h>
#include <iostream>
#include <cmath>



//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;



int main(int argc, char* args[])
{
    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    SDL_Surface* gXOut = NULL;

    //The window we'll be rendering to
    SDL_Window* window = NULL;

    //The surface contained by the window
    SDL_Surface* screenSurface = NULL;

    SDL_GameController* controller = NULL;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }



    //Create window
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    //Get window surface
    screenSurface = SDL_GetWindowSurface(window);

    //Fill the surface white
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

    //Update the surface
    SDL_UpdateWindowSurface(window);


    //Wait 2 seconds
    SDL_Delay(2000);


    SDL_GameControllerEventState(SDL_ENABLE);
    //While application is running
    while (!quit)
    {
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            switch (e.type) {
            case SDL_KEYDOWN:
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_CONTROLLERDEVICEADDED:
                // open the controller to the game controller instance to manipulate it later
                controller = SDL_GameControllerOpen(e.cdevice.which);
                //print out its name to the console
                std::cout << "Controller name: " << SDL_GameControllerNameForIndex(e.cdevice.which) << "\n";
                //if there was an error opening the controller, print out the error
                if (!SDL_GameControllerGetAttached(controller)) {
                    std::cout << "SDL_GetError() = " << SDL_GetError() << "\n";
                }
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                std::cout << "CONTROLLER REMOVED\n";
                break;
            case SDL_CONTROLLERBUTTONDOWN:
                std::cout << "Button " << unsigned(e.cbutton.button) << " Pressed\n";
                break;
            case SDL_CONTROLLERAXISMOTION:
                if (e.caxis.axis == 2 || e.caxis.axis == 3) {
                    std::cout << "Magnitude = " << hypot(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX), SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY)) << "\n";
                }
                else {
                    std::cout << "Axis " << unsigned(e.caxis.axis) << " moved(" << e.caxis.value << ")\n";

                }
                break;
            }
        }
        //Apply the image
        SDL_BlitSurface(gXOut, NULL, screenSurface, NULL);

        //Update the surface
        SDL_UpdateWindowSurface(window);
    }

    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}
*/