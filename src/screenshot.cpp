/**
 * Ashita - Copyright (c) 2023 Ashita Development Team
 * Contact: https://www.ashitaxi.com/
 * Contact: https://discord.gg/Ashita
 *
 * This file is part of Ashita.
 *
 * Ashita is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Ashita is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Ashita.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "screenshot.hpp"
#include <lodepng.h>

namespace screenshot
{
    plugin::plugin(void)
        : core_{nullptr}
        , log_{nullptr}
        , device_{nullptr}
        , id_{0}
    {}
    plugin::~plugin(void)
    {}

    auto plugin::GetName(void) const -> const char*
    {
        return "screenshot";
    }

    auto plugin::GetAuthor(void) const -> const char*
    {
        return "atom0s";
    }

    auto plugin::GetDescription(void) const -> const char*
    {
        return "Saves screenshots of the game.";
    }

    auto plugin::GetLink(void) const -> const char*
    {
        return "https://www.ashitaxi.com/";
    }

    auto plugin::GetVersion(void) const -> double
    {
        return 4.1f;
    }

    auto plugin::GetInterfaceVersion(void) const -> double
    {
        return ASHITA_INTERFACE_VERSION;
    }

    auto plugin::GetPriority(void) const -> int32_t
    {
        return 0;
    }

    auto plugin::GetFlags(void) const -> uint32_t
    {
        return static_cast<uint32_t>(Ashita::PluginFlags::UseCommands | Ashita::PluginFlags::UseDirect3D);
    }

    auto plugin::Initialize(IAshitaCore* core, ILogManager* logger, const uint32_t id) -> bool
    {
        this->core_ = core;
        this->log_  = logger;
        this->id_   = id;

        this->state_.reset();

        char path[MAX_PATH]{};
        ::sprintf_s(path, "%s\\screenshots\\", core->GetInstallPath());

        std::error_code err;
        if (const auto ret = std::filesystem::create_directories(path, err); !ret && err.value() > 0)
        {
            std::ostringstream msg;
            msg << Ashita::Chat::Header("screenshot")
                << Ashita::Chat::Error("Failed to create 'screenshot' directory. Error: %d");
            core->GetChatManager()->Writef(1, false, msg.str().c_str(), ret);

            return false;
        }

        return true;
    }

    auto plugin::Release(void) -> void
    {
    }

    auto plugin::HandleCommand(int32_t mode, const char* command, bool injected) -> bool
    {
        UNREFERENCED_PARAMETER(mode);
        UNREFERENCED_PARAMETER(injected);

        std::vector<std::string> args{};
        const auto count = Ashita::Commands::GetCommandArgs(command, &args);

        HANDLECOMMAND("/screenshot")
        {
            this->state_.reset();

            for (auto x = 1u; x < count; x++)
            {
                if (args[x] == "bmp")
                    this->state_.type_ = screenshot::type::bmp;
                if (args[x] == "png")
                    this->state_.type_ = screenshot::type::png;
                if (args[x] == "hide")
                    this->state_.hide_ = true;
                if (args[x] == "silent")
                    this->state_.silent_ = true;
            }

            this->state_.take_  = true;
            this->state_.stage_ = screenshot::stage::begin;

            return true;
        }

        return false;
    }

    auto plugin::Direct3DInitialize(IDirect3DDevice8* device) -> bool
    {
        this->device_ = device;

        return true;
    }

    auto plugin::Direct3DBeginScene(const bool is_rendering_backbuffer) -> void
    {
        if (!is_rendering_backbuffer)
            return;

        if (!this->state_.take_ || this->state_.stage_ != screenshot::stage::begin)
            return;

        if (this->state_.hide_)
        {
            this->core_->GetGuiManager()->SetVisible(false);
            this->core_->GetFontManager()->SetVisible(false);
            this->core_->GetPrimitiveManager()->SetVisible(false);
        }

        this->state_.stage_ = screenshot::stage::end;
    }

    auto plugin::Direct3DPresent(const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) -> void
    {
        UNREFERENCED_PARAMETER(pSourceRect);
        UNREFERENCED_PARAMETER(pDestRect);
        UNREFERENCED_PARAMETER(hDestWindowOverride);
        UNREFERENCED_PARAMETER(pDirtyRegion);

        if (!this->state_.take_ || this->state_.stage_ != screenshot::stage::end)
            return;

        if (this->state_.hide_)
        {
            this->core_->GetGuiManager()->SetVisible(true);
            this->core_->GetFontManager()->SetVisible(true);
            this->core_->GetPrimitiveManager()->SetVisible(true);
        }

        IDirect3DSurface8* surface = nullptr;
        if (const auto res = this->device_->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surface); FAILED(res))
        {
            std::ostringstream msg;
            msg << Ashita::Chat::Header("screenshot")
                << Ashita::Chat::Error("Failed to obtain the back buffer surface. Error: %08X");
            this->core_->GetChatManager()->Writef(1, false, msg.str().c_str(), res);

            this->state_.reset();
            return;
        }

        this->take_screenshot(surface);

        SAFE_RELEASE(surface);

        this->state_.reset();
    }

    auto plugin::take_screenshot(IDirect3DSurface8* surface) -> void
    {
        UNREFERENCED_PARAMETER(surface);

        const auto create_path = [this](void) -> std::string {
            SYSTEMTIME stime{};
            ::GetLocalTime(&stime);

            char path[MAX_PATH]{};
            ::sprintf_s(path, "%s\\screenshots\\%s_%04d.%02d.%02d_%02d%02d%02d",
                this->core_->GetInstallPath(),
                this->core_->GetMemoryManager()->GetParty()->GetMemberName(0),
                stime.wYear, stime.wMonth, stime.wDay, stime.wHour, stime.wMinute, stime.wSecond);

            switch (this->state_.type_)
            {
                case screenshot::type::bmp:
                    ::strcat_s(path, ".bmp");
                    break;
                case screenshot::type::png:
                    ::strcat_s(path, ".png");
                    break;
            }

            return path;
        };

        const auto save_bmp = [this, &surface](const std::string& path) -> bool {
            if (surface == nullptr || path.size() == 0)
            {
                std::ostringstream msg;
                msg << Ashita::Chat::Header("screenshot")
                    << Ashita::Chat::Error("Invalid screenshot attempt; this should not happen! (.bmp)");
                this->core_->GetChatManager()->Writef(1, false, msg.str().c_str());

                return false;
            }

            if (const auto res = ::D3DXSaveSurfaceToFileA(path.c_str(), D3DXIMAGE_FILEFORMAT::D3DXIFF_BMP, surface, nullptr, nullptr); FAILED(res))
            {
                std::ostringstream msg;
                msg << Ashita::Chat::Header("screenshot")
                    << Ashita::Chat::Error("Failed to save current screenshot surface! (.bmp) Error: %08X");
                this->core_->GetChatManager()->Writef(1, false, msg.str().c_str(), res);

                return false;
            }

            return true;
        };

        const auto save_png = [this, &surface](const std::string& path) -> bool {
            if (surface == nullptr || path.size() == 0)
            {
                std::ostringstream msg;
                msg << Ashita::Chat::Header("screenshot")
                    << Ashita::Chat::Error("Invalid screenshot attempt; this should not happen! (.png)");
                this->core_->GetChatManager()->Writef(1, false, msg.str().c_str());

                return false;
            }

            D3DSURFACE_DESC desc{};
            D3DLOCKED_RECT rect{};

            auto res = surface->GetDesc(&desc);
            if (FAILED(res))
            {
                std::ostringstream msg;
                msg << Ashita::Chat::Header("screenshot")
                    << Ashita::Chat::Error("Failed to save current screenshot surface! (.png - 1) Error: %08X");
                this->core_->GetChatManager()->Writef(1, false, msg.str().c_str(), res);

                return false;
            }

            res = surface->LockRect(&rect, nullptr, D3DLOCK_READONLY);
            if (FAILED(res))
            {
                std::ostringstream msg;
                msg << Ashita::Chat::Header("screenshot")
                    << Ashita::Chat::Error("Failed to save current screenshot surface! (.png - 2) Error: %08X");
                this->core_->GetChatManager()->Writef(1, false, msg.str().c_str(), res);

                return false;
            }

            std::vector<uint8_t> pixels;
            for (auto y = 0u; y < desc.Height; ++y)
            {
                auto p = reinterpret_cast<int32_t*>(static_cast<uint8_t*>(rect.pBits) + (y * rect.Pitch));
                for (auto x = 0u; x < desc.Width; ++x)
                {
                    pixels.push_back((*p >> 0x10) & 0xFF);
                    pixels.push_back((*p >> 0x08) & 0xFF);
                    pixels.push_back((*p >> 0x00) & 0xFF);

                    *p++;
                }
            }

            surface->UnlockRect();

            if (const auto ret = lodepng::encode(path.c_str(), pixels, desc.Width, desc.Height, LCT_RGB); ret != 0)
            {
                std::ostringstream msg;
                msg << Ashita::Chat::Header("screenshot")
                    << Ashita::Chat::Error("Failed to save current screenshot surface! (.png - 3) Error: %d - %s");
                this->core_->GetChatManager()->Writef(1, false, msg.str().c_str(), ret, lodepng_error_text(ret));

                return false;
            }

            return true;
        };

        const auto path = create_path();
        auto ret        = false;

        switch (this->state_.type_)
        {
            case screenshot::type::bmp:
                ret = save_bmp(path);
                break;
            case screenshot::type::png:
                ret = save_png(path);
                break;
        }

        if (!ret || this->state_.silent_)
            return;

        std::ostringstream msg;
        msg << Ashita::Chat::Header("screenshot")
            << Ashita::Chat::Message("Screenshot saved to: ")
            << Ashita::Chat::Success("%s");
        this->core_->GetChatManager()->Writef(1, false, msg.str().c_str(), path.c_str());
    }

} // namespace screenshot

__declspec(dllexport) auto __stdcall expCreatePlugin(const char* args) -> IPlugin*
{
    UNREFERENCED_PARAMETER(args);

    return new screenshot::plugin();
}

__declspec(dllexport) auto __stdcall expGetInterfaceVersion(void) -> double
{
    return ASHITA_INTERFACE_VERSION;
}