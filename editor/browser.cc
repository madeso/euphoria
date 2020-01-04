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
        : current_folder(core::vfs::DirPath::FromRoot())
        , file_system(fs)
    {}

    std::optional<core::vfs::FilePath>
    FileBrowser::GetSelectedFile()
    {
        if(selected_file >= 0 && selected_file < Csizet_to_int(files.size()))
        {
            const auto suggested = files[selected_file];
            if(suggested.is_file)
            {
                return current_folder.GetFile(suggested.name);
            }
        }
        return std::nullopt;
    }

    void
    FileBrowser::SelectFile(const std::string& p)
    {
        #if 0
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
        #endif
    }

    void
    FileBrowser::Refresh()
    {
        files = file_system->ListFiles(current_folder);
        if(current_folder == core::vfs::DirPath::FromRoot())
        {
            files.insert(files.begin(), vfs::ListedFile {"./../", true, false});
        }
        // files.insert(files.begin(), f.begin(), f.end());
        selected_file = -1;
    }

    bool
    FileBrowser::Run()
    {
        bool doubleclicked_file = false;
        window::InputText("URL", &current_folder.path);
        window::InputText("Filter", &filter);
        // ImGui::PushItemWidth(-1);
        ImGui::ListBoxHeader("", ImVec2 {-1, -1});
        int        index = 0;
        // if files is updated during iteration, bad things
        // will probably happen, so we iterate a copy
        const auto ff = files;
        for(const auto& item: ff)
        {
            const bool custom  = item.is_builtin;
            bool display = true;

            if(item.is_file)
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
                    ImGui::PushStyleColor
                    (
                        ImGuiCol_Text,
                        ImColor{0, 0, 255}.Value
                    );
                }

                // todo(Gustav): add file/directory icon
                const auto left_clicked_item = ImGui::Selectable(item.name.c_str(), index == selected_file);
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
                            current_folder = current_folder.GetParentDirectory();
                        }
                        else
                        {
                            const auto sub_directory = core::vfs::DirPath{"./" + item.name};
                            const auto resolved = ResolveRelative(sub_directory, current_folder);
                            ASSERTX(resolved.has_value(), sub_directory, current_folder);
                            current_folder = resolved.value();
                        }
                        Refresh();
                    }
                    else
                    {
                        doubleclicked_file = true;
                    }
                }
            }
            index += 1;
        }
        ImGui::ListBoxFooter();
        // ImGui::PopItemWidth();

        return doubleclicked_file;
    }
}  // namespace euphoria::editor
