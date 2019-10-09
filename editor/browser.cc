#include "editor/browser.h"

#include "imgui/imgui.h"

#include "core/os.h"
#include "core/vfs_path.h"
#include "core/stringutils.h"
#include "core/cint.h"

#include "window/imgui_ext.h"

using namespace euphoria::core;

namespace euphoria::editor
{
    template <typename T, typename G>
    int
    IndexOf(const std::vector<T>& ss, const G& finder)
    {
        int index = 0;
        for(const auto& s: ss)
        {
            if(finder(s))
            {
                return index;
            }
            index += 1;
        }

        return -1;
    }

    FileBrowser::FileBrowser(vfs::FileSystem* fs)
        : current_folder(vfs::Path::FromRoot().GetAbsolutePath())
        , file_system(fs)
    {}

    std::string
    FileBrowser::GetSelectedFile()
    {
        if(selected_file >= 0 && selected_file < Csizet(files.size()))
        {
            const auto suggested = files[selected_file];
            if(!EndsWith(suggested.name, '/'))
            {
                if(current_folder.empty())
                {
                    return suggested.name;
                }
                else
                {
                    return current_folder + suggested.name;
                }
            }
        }
        return "";
    }

    void
    FileBrowser::SelectFile(const std::string& p)
    {
        const auto file = vfs::Path::FromFile(p);
        current_folder  = file.GetDirectory().GetAbsolutePath();
        Refresh();
        if(!p.empty())
        {
            selected_file
                    = IndexOf(files, [&](const vfs::ListedFile& n) -> bool {
                          return n.name == file.GetFileName();
                      });
        }
    }

    void
    FileBrowser::Refresh()
    {
        files = file_system->ListFiles(
                vfs::Path::FromDirectory(current_folder));
        if(!current_folder.empty())
        {
            // files.emplace_back();
            files.insert(files.begin(), vfs::ListedFile {"../", true});
        }
        // files.insert(files.begin(), f.begin(), f.end());
        selected_file = -1;
    }

    bool
    FileBrowser::Run()
    {
        bool selected = false;
        window::InputText("URL", &current_folder);
        window::InputText("Filter", &filter);
        // ImGui::PushItemWidth(-1);
        ImGui::ListBoxHeader("", ImVec2 {-1, -1});
        int        index = 0;
        const auto ff
                = files;  // if files is updated during iteration, bad things
        // will probably happen, so we iterate a copy
        for(const auto& item: ff)
        {
            const bool custom  = item.is_builtin;
            bool       display = true;

            if(!EndsWith(item.name, '/'))
            {
                if(!filter.empty())
                {
                    if(!EndsWith(item.name, filter))
                    {
                        display = false;
                    }
                }
            }

            if(display)
            {
                if(custom)
                {
                    ImGui::PushStyleColor(
                            ImGuiCol_Text, ImColor {0, 0, 255}.Value);
                }
                if(ImGui::Selectable(item.name.c_str(), index == selected_file))
                {
                    selected_file = index;
                }
                if(ImGui::IsItemHovered(
                           ImGuiHoveredFlags_AllowWhenBlockedByPopup)
                   && ImGui::IsMouseClicked(1))
                {
                    // also select when right click
                    selected_file = index;
                }
                if(custom)
                {
                    ImGui::PopStyleColor();
                }

                if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    if(EndsWith(item.name, '/'))
                    {
                        if(item.name == "../")
                        {
                            current_folder
                                    = vfs::Path::FromDirectory(current_folder)
                                              .GetParentDirectory()
                                              .GetAbsolutePath();
                        }
                        else
                        {
                            current_folder += item.name;
                        }
                        Refresh();
                    }
                    else
                    {
                        selected = true;
                    }
                }
            }
            index += 1;
        }
        ImGui::ListBoxFooter();
        // ImGui::PopItemWidth();

        return selected;
    }
}  // namespace euphoria::editor
