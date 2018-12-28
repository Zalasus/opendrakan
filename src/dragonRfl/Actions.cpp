/*
 * Actions.cpp
 *
 *  Created on: 28 Dec 2018
 *      Author: zal
 */

#include <dragonRfl/Actions.h>

namespace dragonRfl
{

    const std::map<Action, od::RecordId> sActionNameMap
    {
        { Action::Forward,                      0x4019 }, // Vorwärts
        { Action::Backward,                     0x4020 }, // Rückwärts
        { Action::Strafe_Left,                  0x4013 }, // Nach links
        { Action::Strafe_Right,                 0x4014 }, // Nach rechts
        { Action::Jump,                         0x4011 }, // Springen/hoch
        { Action::Duck,                         0x4012 }, // Ducken/runter
        { Action::Turn_Left,                    0x4015 }, // Nach links drehen
        { Action::Turn_Right,                   0x4016 }, // Nach rechts drehen
        { Action::Tilt_Up,                      0x4017 }, // Nach oben neigen
        { Action::Tilt_Down,                    0x4018 }, // Nach unten neigen
        { Action::Attack_Primary,               0x4006 }, // Primärangriff
        { Action::Attack_Secondary,             0x4007 }, // Alternativangriff
        { Action::Free_Look,                    0x400a }, // Freies Umsehen
        { Action::Crouch,                       0x4010 }, // Schleichen/Kamera
        { Action::Interact,                     0x4009 }, // Gegenstand benutzen
        { Action::Dragon_Action,                0x4008 }, // Drachen-Aktion
        { Action::Use_Potion_Health,            0x4036 }, // Heiltrank benutzen
        { Action::Use_Potion_Invisibility,      0x4037 }, // Unsichtbarkeitstrank b.
        { Action::Use_Potion_Invulnerability,   0x4038 }, // Unverwundbarkeitstrank b.
        { Action::Use_Potion_Speed,             0x4039 }, // Geschwindigkeitstrank b.
        { Action::Weapon_Next,                  0x4035 }, // Nächste Waffe
        { Action::Weapon_Previous,              0x4034 }, // Vorherige Waffe
        { Action::Weapon_Stow,                  0x403a }, // Waffe verstauen
        { Action::Inventory,                    0x4030 }, // Inventar
        { Action::Pause,                        0x4001 }, // Spiel unterbrechen
        { Action::Save_Quick,                   0x4002 }, // Schnellspeichern
        { Action::Load_Quick,                   0x4003 }, // Schnellladen
        { Action::Map,                          0x4031 }, // Karte an/aus
        { Action::Enginedata_Toggle,            0x4032 }, // Engine-Daten an/aus
        { Action::Change_Teams,                 0x4057 }, // Teams ändern
        { Action::Chat,                         0x4056 }, // Netzwerk-Chat
        { Action::Main_Menu,                    0x4040 }, // Hauptmenü
        { Action::Options,                      0x4043 }, // Optionen
        { Action::Save,                         0x4044 }, // Spielstand speichern
        { Action::Load,                         0x4045 }, // Spielstand laden
        { Action::Fullscreen_Toggle,            0x4050 }, // Vollbild an/aus
        { Action::Resolution_Up,                0x4051 }, // Auflösung erhöhen
        { Action::Resolution_Down,              0x4052 }, // Auflösung verringern
        { Action::Fogdepth_Up,                  0x4053 }, // Nebeltiefe erhöhen
        { Action::Fogdepth_Down,                0x4054 }, // Nebeltiefe verringern
        { Action::Screenshot,                   0x4055 }, // Bildschirm speichern
        { Action::Demo_Record,                  0x4004 }, // Demo aufnehmen
        { Action::Demo_Playback,                0x4005 }, // Demo abspielen
    };

}
