#include "PieMenu.h"
// Return >= 0 on mouse release
// Optional int* p_selected display and update a currently selected item
int lastHovered = -1;
bool gamepadActivated = false;
int PiePopupSelectMenu(const ImVec2& center, const char* popup_id, std::vector<EquipmentStruct> items, int* p_selected,  int* p_selected2)
{
    int ret = -1;

    // FIXME: Missing a call to query if Popup is open so we can move the PushStyleColor inside the BeginPopupBlock (e.g. IsPopupOpen() in imgui.cpp)
    // FIXME: Our PathFill function only handle convex polygons, so we can't have items spanning an arc too large else inner concave edge artifact is too visible, hence the ImMax(7,items_count)
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    if (ImGui::BeginPopup(popup_id))
    {
        ImGui::PushFontShadow(0xFF000000);
        playOpenCloseSound(false);
        ImVec2 drag_delta = ImVec2(ImGui::GetIO().MousePos.x - center.x, ImGui::GetIO().MousePos.y - center.y);
        float gamepadX = ImGui::GetKeyData(ImGuiKey_GamepadRStickRight)->AnalogValue - ImGui::GetKeyData(ImGuiKey_GamepadRStickLeft)->AnalogValue;
        float gamepadY = ImGui::GetKeyData(ImGuiKey_GamepadRStickDown)->AnalogValue - ImGui::GetKeyData(ImGuiKey_GamepadRStickUp)->AnalogValue;
  
        bool usingGamepad = false;
        if (fabs(gamepadX) > 0.6 || fabs(gamepadY) > 0.6)
        {
            drag_delta = ImVec2(gamepadX, gamepadY);
            usingGamepad = true;
        }
        const float drag_dist2 = drag_delta.x * drag_delta.x + drag_delta.y * drag_delta.y;

        const float pixel_ratio = ImGui::GetIO().DisplaySize.y / 900;
        const ImGuiStyle& style = ImGui::GetStyle();
        const float RADIUS_MIN = 220.0f * pixel_ratio;
        const float RADIUS_MAX = 380.0f * pixel_ratio;
        const float RADIUS_INTERACT_MIN = 100.0f * pixel_ratio;
        const int ITEMS_MIN = 6;

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        //ImGuiWindow* window = ImGui::GetCurrentWindow();
        draw_list->PushClipRectFullScreen();
        draw_list->PathArcTo(center, (RADIUS_MIN + RADIUS_MAX) * 0.5f, 0.0f, IM_PI * 2.0f * 0.99f, 32);   // FIXME: 0.99f look like full arc with closed thick stroke has a bug now
        draw_list->PathStroke(ImColor(0, 0, 0, 0), true, RADIUS_MAX - RADIUS_MIN);

        const float item_arc_span = 2 * IM_PI / ImMax(ITEMS_MIN, (int) items.size());
        float drag_angle = atan2f(drag_delta.y, drag_delta.x);
        if (drag_angle < -0.5f * item_arc_span)
            drag_angle += 2.0f * IM_PI;
        //ImGui::Text("%f", drag_angle);    // [Debug]

        int item_hovered = -1;
        for (int item_n = 0; item_n < items.size(); item_n++)
        {
            std::string item_name = items[item_n].getName();
            const char* item_label = item_name.c_str();
            const float inner_spacing = style.ItemInnerSpacing.x / RADIUS_MIN / 2;
            const float item_inner_ang_min = item_arc_span * (item_n - 0.5f);
            const float item_inner_ang_max = item_arc_span * (item_n + 0.5f);
            const float item_outer_ang_min = item_arc_span * (item_n - 0.5f * (RADIUS_MIN / RADIUS_MAX));
            const float item_outer_ang_max = item_arc_span * (item_n + 0.5f * (RADIUS_MIN / RADIUS_MAX));

            bool hovered = false;

            if (((drag_angle >= item_inner_ang_min && drag_angle < item_inner_ang_max) && (drag_dist2 >= RADIUS_INTERACT_MIN * RADIUS_INTERACT_MIN || usingGamepad) ) || item_n == lastHovered ) {
                hovered = true;
                playHoverSound(item_n);
                lastHovered = item_n;
            }
            
            //adds the inner spacing to draw between elements. I removed it from the originals so there weren't awkward gaps for gamepads
            const float visual_item_inner_ang_min = item_arc_span * (item_n - 0.5f + inner_spacing);
            const float visual_item_inner_ang_max = item_arc_span * (item_n + 0.5f - inner_spacing);
            const float visual_item_outer_ang_min = item_arc_span * (item_n - 0.5f + inner_spacing * (RADIUS_MIN / RADIUS_MAX));
            const float visual_item_outer_ang_max = item_arc_span * (item_n + 0.5f - inner_spacing * (RADIUS_MIN / RADIUS_MAX));

            bool selected = (p_selected && (*p_selected == item_n)) || (p_selected2 && (*p_selected2 == item_n));
            int image_size_offset = 50 * pixel_ratio;
            int image_upshift = 9 * pixel_ratio;
            int textOffset = 50 * pixel_ratio;
            //printf("art: %d    pros:%d\n", *p_selected, *p_selected2);
            int arc_segments = (int)(32 * item_arc_span / (2 * IM_PI)) + 1;
            draw_list->PathArcTo(center, RADIUS_MAX - style.ItemInnerSpacing.x, visual_item_outer_ang_min, visual_item_outer_ang_max, arc_segments);
            draw_list->PathArcTo(center, RADIUS_MIN + style.ItemInnerSpacing.x, visual_item_inner_ang_max, visual_item_inner_ang_min, arc_segments);
            //draw_list->PathFill(window->Color(hovered ? ImGuiCol_HeaderHovered : ImGuiCol_FrameBg));
            draw_list->PathFillConvex(hovered ? ImColor(96, 80, 62, 125) : selected ? ImColor(96, 80, 62, 200) : ImColor( 0, 0, 0, 160));
            ImVec2 text_size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, item_label);
            ImVec2 text_pos = ImVec2(  //make room for image
                center.x + cosf((item_inner_ang_min + item_inner_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size.x * 0.5f,
                center.y + sinf((item_inner_ang_min + item_inner_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size.y * 0.5f);

            ImVec2 image_pos = ImVec2(
                center.x + cosf((item_inner_ang_min + item_inner_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f,
                center.y + sinf((item_inner_ang_min + item_inner_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f);

            if (items[item_n].realID == 5000 || items[item_n].realID == 110000)
            {
                draw_list->AddText(text_pos, ImColor(255, 255, 255), item_label);
            }
            else {
                ImageInfoStruct* image = &items[item_n].image;
                draw_list->AddImage(image->imageptr, ImVec2(image_pos.x - image_size_offset, image_pos.y - image_size_offset - image_upshift ), ImVec2(image_pos.x + image_size_offset, image_pos.y + image_size_offset - image_upshift), image->uv0, image->uv1);

                std::istringstream iss(item_label);
                std::string s;
                std::string temp;
                std::vector<std::string> textSplit;
                int spaceCounter = 0;
                while (true) {
                    if (std::getline(iss, s, ' '))
                    {
                        if (spaceCounter < 2)
                        {
                            temp += s + " ";
                            spaceCounter++;
                        }
                        else {
                            textSplit.push_back(temp);
                            temp = s + " ";
                            spaceCounter = 0;
                        }
                    }
                    else {
                        textSplit.push_back(temp);
                        break;
                    }

                }

                for (size_t i = 0; i < textSplit.size(); i++)
                {
                    std::string b = textSplit[i];
                    ImVec2 text_size2 = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.0f, b.c_str());
                    ImVec2 text_pos2 = ImVec2(  //make room for image
                        center.x + cosf((item_inner_ang_min + item_inner_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size2.x * 0.5f,
                        center.y + sinf((item_inner_ang_min + item_inner_ang_max) * 0.5f) * (RADIUS_MIN + RADIUS_MAX) * 0.5f - text_size2.y * 0.5f + textOffset + (text_size2.y*i));
                    draw_list->AddText(text_pos2, ImColor(255, 255, 255), b.c_str());

                }



            }

            if (hovered)
                item_hovered = item_n;
        }
        draw_list->PopClipRect();

        if (ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_Tab)) || ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_GamepadDpadDown)))
        {
            ImGui::CloseCurrentPopup();
            lastHovered = -1;
            ret = item_hovered;
            playOpenCloseSound(true);
            resetSound();
            
            Hooks::setInputCapture();
            if (p_selected && item_hovered != -1) {
                if (items[item_hovered].realID < 10000)
                    *p_selected = item_hovered;
                else if (items[item_hovered].realID > 10000)
                    *p_selected2 = item_hovered;
            }

        }
        ImGui::EndPopup();
    }
    ImGui::PopFontShadow();
    ImGui::PopStyleColor(2);
    return ret;
}

int lastPlayed = -1;
void playHoverSound(int index) {
    if (index != lastPlayed) {
        GUISoundMaker9000(0x2b, 0x2710);
        lastPlayed = index;
    }
}

bool alreadyOpened = false;
void playOpenCloseSound(bool close) {
    if (!alreadyOpened || close)
    {
        GUISoundMaker9000(0x29, 0x1F4);
        alreadyOpened = true;
    }
}

void resetSound() {
    lastPlayed = -1;
    alreadyOpened = false;
}