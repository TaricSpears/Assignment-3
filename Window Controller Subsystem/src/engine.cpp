#include <string>
#include "crow.h"
#include <iostream>
#include <fstream>

void send_to_serial(const std::string &msg)
{
    std::ofstream serial("COM3"); // ⚠️ Modifica con la tua porta COM di Arduino
    if (serial.is_open())
    {
        serial << msg << "\n";
        serial.close();
    }
    else
    {
        std::cerr << "Errore apertura COM\n";
    }
}

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/mode").methods("POST"_method)([](const crow::request &req)
                                                    {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400);
        std::string mode = body["mode"].s();
        send_to_serial("MODE " + mode);
        return crow::response(200); });

    CROW_ROUTE(app, "/window").methods("POST"_method)([](const crow::request &req)
                                                      {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400);
        int p = std::clamp(body["percentage"].i(), 0, 100);
        send_to_serial("SET_AUTO " + std::to_string(p));
        return crow::response(200); });

    std::cout << "Server in ascolto su http://localhost:18080\n";
    app.port(18080).multithreaded().run();
}
