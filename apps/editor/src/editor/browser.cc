#include "editor/browser.h"

#include "imgui/imgui.h"
#include "imgui_stdlib.h"

#include "base/os.h"
#include "io/vfs_path.h"
#include "base/stringutils.h"
#include "base/cint.h"
#include "log/log.h"

#include "window/imgui_extra.h"
#include "window/imgui_icons.h"

#include <optional>

using namespace eu;
using namespace eu::core;

namespace eu::editor
{
    template <typename T, typename G>
    int
    index_of(const std::vector<T>& container, const G& finder)
    {
        int index = 0;
        for(const auto& s: container)
        {
            if(finder(s))
            {
                return index;
            }
            index += 1;
        }

        return -1;
    }

    FileBrowser::FileBrowser(io::FileSystem* a_file_system)
        : current_folder(io::DirPath::from_root())
        , file_system(a_file_system)
    {
    }

    std::optional<io::FilePath>
    FileBrowser::get_selected_file()
    {
        if(selected_file >= 0 && selected_file < c_sizet_to_int(files.size()))
        {
            const auto suggested = files[selected_file];
            if(suggested.is_file)
            {
                return current_folder.get_file(suggested.name);
            }
        }
        return std::nullopt;
    }

    void
    FileBrowser::refresh()
    {
        files = file_system->list_files(current_folder);
        if(current_folder != io::DirPath::from_root())
        {
            files.insert(files.begin(), io::ListedFile {"../", true, false});
        }
        selected_file = -1;
    }

    std::string
    determine_icon_string(const io::ListedFile& file)
    {
        if(file.is_file)
        {
            const auto x = io::FilePath{"./" + file.name}.get_extension();
            if(x == "png")
            {
                return ICON_MD_IMAGE;
            }
            if(x == "js" || x=="frag" || x=="vert")
            {
                return ICON_MD_SOURCE;
            }
            if(x == "json")
            {
                return ICON_MD_SUBJECT;
            }
            if(x == "mp4")
            {
                return ICON_MD_MOVIE;
            }
            if(x == "mp3")
            {
                return ICON_MD_MUSIC_NOTE;
            }
            return ICON_MD_DESCRIPTION;
        }
        else
        {
            return ICON_MD_FOLDER;
        }

    }

    bool
    FileBrowser::run()
    {
        ImGui::InputText("URL", &current_folder.path);
        ImGui::InputText("Filter", &filter);
        if(ImGui::BeginListBox("##files", ImVec2 {-1, -1}) == false)
        {
            return false;
        }

        int index = 0;
        bool doubleclicked_file = false;

        // if files is updated during iteration, bad things
        // will probably happen, so we iterate a copy
        const auto ff = files;

        for(const auto& item: ff)
        {
            const bool custom = item.is_builtin;
            bool display = true;

            if(item.is_file)
            {
                if(!filter.empty())
                {
                    if(!ends_with(item.name, filter))
                    {
                        display = false;
                    }
                }
            }

            if(display)
            {
                if(custom)
                {
                    ImGui::PushStyleColor
                    (
                        ImGuiCol_Text,
                        ImColor{0, 0, 255}.Value
                    );
                }

                const auto icon = determine_icon_string(item);
                const auto icon_and_name = icon + item.name;
                const auto left_clicked_item = ImGui::Selectable
                (
                    icon_and_name.c_str(),
                    index == selected_file
                );
                const auto user_clicked_right = ImGui::IsMouseClicked(1);
                const auto mouse_hovering_item = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);
                const auto right_clicked_item = mouse_hovering_item && user_clicked_right;

                if(left_clicked_item || right_clicked_item)
                {
                    selected_file = index;
                }
                if(custom)
                {
                    ImGui::PopStyleColor();
                }

                if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    if(item.is_file == false)
                    {
                        if(item.name == "../")
                        {
                            current_folder = current_folder.get_parent_directory();
                        }
                        else
                        {
                            const auto sub_directory = io::DirPath{"./" + item.name};
                            const auto resolved = resolve_relative(sub_directory, current_folder);
                            ASSERTX(resolved.has_value(), sub_directory, current_folder);
                            current_folder = resolved.value();
                        }
                        refresh();
                    }
                    else
                    {
                        doubleclicked_file = true;
                    }
                }
            }
            index += 1;
        }
        ImGui::EndListBox();
        // ImGui::PopItemWidth();

        return doubleclicked_file;
    }
}
