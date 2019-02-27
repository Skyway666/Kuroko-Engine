#include "UI_HierarchyTab.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "GameObject.h"

void UI_HierarchyTab::UpdateContent()
{

	current_id = 0;
	std::list<GameObject*> root_objs;
	App->scene->getRootObjs(root_objs);

	bool item_hovered = false;

	for (auto it = root_objs.begin(); it != root_objs.end(); it++)
	{
		if (DrawHierarchyNode(*(*it)))
			item_hovered = true;

		current_id++;
	}

	if (ImGui::IsWindowHovered())
	{
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			App->scene->selected_obj = nullptr;
		else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN && !item_hovered)
			ImGui::OpenPopup("##hierarchy context menu");
	}

	if (ImGui::BeginPopup("##hierarchy context menu"))
	{
		if (ImGui::MenuItem("Empty gameobject"))
		{
			GameObject* go = new GameObject("Empty", App->scene->selected_obj);
			if (App->scene->selected_obj)
				App->scene->selected_obj->addChild(go);
		}
		if (ImGui::TreeNode("UI"))
		{
			if (ImGui::MenuItem("UI_Image"))
			{
				GameObject* parent = nullptr;
				if (App->scene->selected_obj) {
					if (App->scene->selected_obj->getComponent(RECTTRANSFORM) != nullptr) {
						parent = App->scene->selected_obj;
					}

				}
				else {
					parent = App->scene->getCanvasGameObject();// creates or checks for the cnavas					
				}
				if (parent != nullptr) {
					GameObject* image = new GameObject("UI_Image", parent, true);
					image->addComponent(Component_type::UI_IMAGE);
					parent->addChild(image);
				}
			}
			if (ImGui::MenuItem("UI_Text"))
			{
				GameObject* parent = nullptr;
				if (App->scene->selected_obj) {
					if (App->scene->selected_obj->getComponent(RECTTRANSFORM) != nullptr) {
						parent = App->scene->selected_obj;
					}
				}
				else {
					parent = App->scene->getCanvasGameObject();// creates or checks for the cnavas					
				}
				if (parent != nullptr) {
					GameObject* text = new GameObject("UI_Text", parent, true);
					text->addComponent(Component_type::UI_TEXT);
					parent->addChild(text);
				}

			}

			if (ImGui::MenuItem("UI_Button"))
			{
				GameObject* parent = nullptr;
				if (App->scene->selected_obj) {
					if (App->scene->selected_obj->getComponent(RECTTRANSFORM) != nullptr) {
						parent = App->scene->selected_obj;
					}
				}
				else {
					parent = App->scene->getCanvasGameObject();// creates or checks for the cnavas					
				}
				if (parent != nullptr) {
					GameObject* button = new GameObject("UI_Button", parent, true);
					button->addComponent(Component_type::UI_IMAGE);
					button->addComponent(Component_type::UI_BUTTON);
					parent->addChild(button);
				}

			}
			if (ImGui::MenuItem("UI_CheckBox"))
			{
				GameObject* parent = nullptr;
				if (App->scene->selected_obj) {
					if (App->scene->selected_obj->getComponent(RECTTRANSFORM) != nullptr) {
						parent = App->scene->selected_obj;
					}
				}
				else {
					parent = App->scene->getCanvasGameObject();// creates or checks for the cnavas					
				}
				if (parent != nullptr) {
					GameObject* chbox = new GameObject("UI_CheckBox", parent, true);
					chbox->addComponent(Component_type::UI_IMAGE);
					chbox->addComponent(Component_type::UI_CHECKBOX);
					parent->addChild(chbox);
				}

			}
			ImGui::TreePop();
		}
		ImGui::EndPopup();
	}
}

bool UI_HierarchyTab::DrawHierarchyNode(GameObject& game_object)
{

	static int selection_mask = (1 << 2);
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << current_id)) ? ImGuiTreeNodeFlags_Selected : 0);

	std::list<GameObject*> children;
	game_object.getChildren(children);

	if (children.empty())
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

	bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)game_object.getUUID(), node_flags, game_object.getName().c_str()) && !children.empty();
	bool item_hovered = ImGui::IsItemHovered();

	if (App->scene->selected_obj == &game_object)
		selection_mask = (1 << current_id);
	else if (App->scene->selected_obj == nullptr)
		selection_mask = (1 >> current_id);


	if (ImGui::IsItemClicked())
		App->scene->selected_obj = &game_object;
	else if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
		ImGui::OpenPopup(("##" + game_object.getName() + std::to_string(current_id) + "Object context menu").c_str());

	static int show_rename = -1;
	if (ImGui::BeginPopup(("##" + game_object.getName() + std::to_string(current_id) + "Object context menu").c_str()))
	{
		if (ImGui::Button(("Duplicate##" + game_object.getName() + std::to_string(current_id) + "Duplicate gobj button").c_str()))
			App->scene->duplicateGameObject(&game_object);

		if (ImGui::Button(("Rename##" + game_object.getName() + std::to_string(current_id) + "Rename gobj button").c_str()))
			show_rename = current_id;

		if (ImGui::Button(("Delete##" + game_object.getName() + std::to_string(current_id) + "Delete gobj button").c_str()))
			App->scene->deleteGameObjectRecursive(&game_object);

		if (ImGui::Button(("Save to prefab##" + game_object.getName() + std::to_string(current_id) + "prefab save gobj button").c_str()))
			App->scene->SavePrefab(&game_object, (game_object.getName()).c_str());

		ImGui::EndPopup();
	}

	if (node_open)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3);

		for (std::list<GameObject*>::iterator it = children.begin(); it != children.end(); it++) 
		{
			if (DrawHierarchyNode(*(*it)))
				item_hovered = true;

			current_id++;
		}

		ImGui::PopStyleVar();
		ImGui::TreePop();
	}

	if (show_rename == current_id)
	{
		static bool rename_open = true;
		App->gui->disable_keyboard_control = true;
		ImGui::SetNextWindowPos(ImVec2(700, 320), ImGuiCond_FirstUseEver);
		ImGui::Begin("Rename object", &rename_open);

		static char rename_buffer[64];
		ImGui::PushItemWidth(ImGui::GetWindowSize().x - 60);
		ImGui::InputText("##Rename to", rename_buffer, 64);

		ImGui::SameLine();
		if (ImGui::Button("OK##Change name"))
		{
			game_object.Rename(rename_buffer);
			show_rename = -1;
		}

		ImGui::End();

		if (!rename_open)
			show_rename = -1;
	}

	return item_hovered;
}