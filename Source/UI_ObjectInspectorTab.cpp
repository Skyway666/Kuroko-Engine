#include "UI_ObjectInspectorTab.h"
#include "ModuleScene.h"
#include "ModuleResourcesManager.h"
#include "ModuleCamera3D.h"
#include "ModuleAudio.h"

#include "GameObject.h"
#include "ComponentScript.h"
#include "ComponentAABB.h"
#include "ComponentAnimation.h"
#include "ComponentAudioListener.h"
#include "ComponentAudioSource.h"
#include "ComponentBone.h"
#include "ComponentCamera.h"
#include "ComponentCanvas.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentButtonUI.h"
#include "ComponentCheckBoxUI.h"
#include "ComponentTextUI.h"
#include "ComponentImageUI.h"
#include "ComponentRectTransform.h"

#include "Material.h"
#include "Camera.h"
#include "ResourceTexture.h"
#include "WinItemDialog.h"
#include "FontManager.h"
#include "UI_ColorPickerWindow.h"

#include <string>
#include <cstring>
#include <fstream>

void UI_ObjectInspectorTab::UpdateContent()
{

	static bool select_script = false;
	GameObject* selected_obj = App->scene->selected_obj;

	if (selected_obj)
	{
		ImGui::Text("Name: %s", selected_obj->getName().c_str());

		ImGui::Checkbox("Active", &selected_obj->is_active);
		ImGui::SameLine();
		if (ImGui::Checkbox("Static", &selected_obj->is_static)) // If an object is set/unset static, reload the quadtree
			App->scene->quadtree_reload = true;

		DrawTagSelection(selected_obj);
		// Add a new tag
		static char new_tag[64];
		ImGui::InputText("New Tag", new_tag, 64);
		if (ImGui::Button("Add Tag")) {
			App->scripting->tags.push_back(new_tag);
			for (int i = 0; i < 64; i++)
				new_tag[i] = '\0';

		}

		if (ImGui::CollapsingHeader("Add component"))
		{
			if (ImGui::Button("Add Mesh"))	selected_obj->addComponent(MESH);
			if (ImGui::Button("Add Camera"))  selected_obj->addComponent(CAMERA);
			if (ImGui::Button("Add Script")) select_script = true;
			if (ImGui::Button("Add Animation")) selected_obj->addComponent(ANIMATION);
		}

		std::list<Component*> components;
		selected_obj->getComponents(components);

		std::list<Component*> components_to_erase;

		for (std::list<Component*>::iterator it = components.begin(); it != components.end(); it++) {
			if (!DrawComponent(*(*it)))
				components_to_erase.push_back(*it);
			current_id++;
		}

		for (std::list<Component*>::iterator it = components_to_erase.begin(); it != components_to_erase.end(); it++)
			selected_obj->removeComponent(*it);

		if (select_script) {
			std::list<resource_deff> script_res;
			App->resources->getScriptResourceList(script_res);

			ImGui::Begin("Script selector", &select_script);
			for (auto it = script_res.begin(); it != script_res.end(); it++) {
				resource_deff script_deff = (*it);
				if (ImGui::MenuItem(script_deff.asset.c_str())) {
					ComponentScript* c_script = (ComponentScript*)selected_obj->addComponent(SCRIPT);
					c_script->assignScriptResource(script_deff.uuid);
					select_script = false;
					break;
				}
			}

			ImGui::End();
		}
	}
}

bool UI_ObjectInspectorTab::DrawComponent(Component& component)
{

	bool ret = true;

	ComponentCamera* camera = nullptr; // aux pointer

	std::string tag;
	ImGui::PushID(component.getUUID());

	switch (component.getType())
	{
	case MESH:
		tag = "Mesh##" + std::to_string(current_id);
		if (ImGui::CollapsingHeader(tag.c_str()))
		{
			ComponentMesh* c_mesh = (ComponentMesh*)&component;
			static bool wireframe_enabled;
			static bool mesh_active;
			static bool draw_normals;

			wireframe_enabled = c_mesh->getWireframe();
			draw_normals = c_mesh->getDrawNormals();
			mesh_active = c_mesh->isActive();

			if (ImGui::Checkbox("Active## mesh_active", &mesh_active))
				c_mesh->setActive(mesh_active);

			if (mesh_active)
			{

				if (ImGui::Checkbox("Wireframe", &wireframe_enabled))
					c_mesh->setWireframe(wireframe_enabled);
				ImGui::SameLine();
				if (ImGui::Checkbox("Draw normals", &draw_normals))
					c_mesh->setDrawNormals(draw_normals);

				if (!c_mesh->getMesh()) {
					static bool add_mesh_menu = false;
					if (ImGui::Button("Add mesh")) {
						add_mesh_menu = true;
					}

					if (add_mesh_menu) {

						std::list<resource_deff> mesh_res;
						App->resources->getMeshResourceList(mesh_res);

						ImGui::Begin("Mesh selector", &add_mesh_menu);
						for (auto it = mesh_res.begin(); it != mesh_res.end(); it++) {
							resource_deff mesh_deff = (*it);
							if (ImGui::MenuItem(mesh_deff.asset.c_str())) {
								App->resources->deasignResource(c_mesh->getMeshResource());
								App->resources->assignResource(mesh_deff.uuid);
								c_mesh->setMeshResourceId(mesh_deff.uuid);
								add_mesh_menu = false;
								break;
							}
						}

						ImGui::End();
					}
				}

				if (Mesh* mesh = c_mesh->getMesh())
				{
					if (ImGui::TreeNode("Mesh Options"))
					{
						uint vert_num, poly_count;
						bool has_normals, has_colors, has_texcoords;
						if (ImGui::Button("Remove mesh")) {
							App->resources->deasignResource(c_mesh->getMeshResource());
							c_mesh->setMeshResourceId(0);
						}
						mesh->getData(vert_num, poly_count, has_normals, has_colors, has_texcoords);
						ImGui::Text("vertices: %d, poly count: %d, ", vert_num, poly_count);
						ImGui::Text(has_normals ? "normals: Yes," : "normals: No,");
						ImGui::Text(has_colors ? "colors: Yes," : "colors: No,");
						ImGui::Text(has_texcoords ? "tex coords: Yes" : "tex coords: No");

						ImGui::TreePop();
					}
				}


				if (ImGui::TreeNode("Material"))
				{
					if (Material* material = c_mesh->getMaterial())
					{
						static int preview_size = 128;
						ImGui::Text("Id: %d", material->getId());
						ImGui::SameLine();
						if (ImGui::Button("remove material"))
						{
							delete c_mesh->getMaterial();
							c_mesh->setMaterial(nullptr);
							ImGui::TreePop();
							return true;
						}

						ImGui::Text("Preview size");
						ImGui::SameLine();
						if (ImGui::Button("64")) preview_size = 64;
						ImGui::SameLine();
						if (ImGui::Button("128")) preview_size = 128;
						ImGui::SameLine();
						if (ImGui::Button("256")) preview_size = 256;

						if (ImGui::TreeNode("diffuse"))
						{
							Texture* texture = nullptr;
							if (ResourceTexture* tex_res = (ResourceTexture*)App->resources->getResource(material->getTextureResource(DIFFUSE)))
								texture = tex_res->texture;


							ImGui::Image(texture ? (void*)texture->getGLid() : (void*)App->gui->ui_textures[NO_TEXTURE]->getGLid(), ImVec2(preview_size, preview_size));
							ImGui::SameLine();

							int w = 0; int h = 0;
							if (texture)
								texture->getSize(w, h);

							ImGui::Text("texture data: \n x: %d\n y: %d", w, h);

							//if (ImGui::Button("Load checkered##Dif: Load checkered"))
							//	material->setCheckeredTexture(DIFFUSE);
							//ImGui::SameLine()
							if (ImGui::Button("Load(from asset folder)##Dif: Load"))
							{
								std::string texture_path = openFileWID();
								uint new_resource = App->resources->getResourceUuid(texture_path.c_str());
								if (new_resource != 0) {
									App->resources->assignResource(new_resource);
									App->resources->deasignResource(material->getTextureResource(DIFFUSE));
									material->setTextureResource(DIFFUSE, new_resource);
								}
							}
							ImGui::TreePop();
						}

						if (ImGui::TreeNode("ambient (feature not avaliable yet)"))
						{
							//ImGui::Image(material->getTexture(AMBIENT) ? (void*)material->getTexture(AMBIENT)->getGLid() : (void*)ui_textures[NO_TEXTURE]->getGLid(), ImVec2(preview_size, preview_size));

							//if (ImGui::Button("Load checkered##Amb: Load checkered"))
							//	material->setCheckeredTexture(AMBIENT);
							//ImGui::SameLine();
							//if (ImGui::Button("Load##Amb: Load"))
							//{
							//	std::string texture_path = openFileWID();
							//	if (Texture* tex = (Texture*)App->importer->Import(texture_path.c_str(), I_TEXTURE))
							//		c_mesh->getMaterial()->setTexture(AMBIENT, tex);
							//}
							ImGui::TreePop();
						}

						if (ImGui::TreeNode("normals (feature not avaliable yet)"))
						{
							//ImGui::Image(material->getTexture(NORMALS) ? (void*)material->getTexture(NORMALS)->getGLid() : (void*)ui_textures[NO_TEXTURE]->getGLid(), ImVec2(preview_size, preview_size));

							//if (ImGui::Button("Load checkered##Nor: Load checkered"))
							//	material->setCheckeredTexture(NORMALS);
							//ImGui::SameLine();
							//if (ImGui::Button("Load##Nor: Load"))
							//{
							//	std::string texture_path = openFileWID();
							//	if (Texture* tex = (Texture*)App->importer->Import(texture_path.c_str(), I_TEXTURE))
							//		c_mesh->getMaterial()->setTexture(NORMALS, tex);
							//}
							ImGui::TreePop();
						}

						if (ImGui::TreeNode("lightmap (feature not avaliable yet)"))
						{
							//ImGui::Image(material->getTexture(LIGHTMAP) ? (void*)material->getTexture(LIGHTMAP)->getGLid() : (void*)ui_textures[NO_TEXTURE]->getGLid(), ImVec2(preview_size, preview_size));

							//if (ImGui::Button("Load checkered##Lgm: Load checkered"))
							//	material->setCheckeredTexture(LIGHTMAP);
							//ImGui::SameLine();
							//if (ImGui::Button("Load##Lgm: Load"))
							//{
							//	std::string texture_path = openFileWID();
							//	if (Texture* tex = (Texture*)App->importer->Import(texture_path.c_str(), I_TEXTURE))
							//		c_mesh->getMaterial()->setTexture(LIGHTMAP, tex);
							//}
							ImGui::TreePop();
						}
					}
					else
					{
						ImGui::TextWrapped("No material assigned!");

						if (c_mesh->getMesh())
						{
							static bool draw_colorpicker = false;
							static Color reference_color = c_mesh->getMesh()->tint_color;
							static GameObject* last_selected = c_mesh->getParent();

							std::string label = c_mesh->getParent()->getName() + " color picker";

							if (last_selected != c_mesh->getParent())
								reference_color = c_mesh->getMesh()->tint_color;

							ImGui::SameLine();
							if (ImGui::ColorButton((label + "button").c_str(), ImVec4(c_mesh->getMesh()->tint_color.r, c_mesh->getMesh()->tint_color.g, c_mesh->getMesh()->tint_color.b, c_mesh->getMesh()->tint_color.a)))
								draw_colorpicker = !draw_colorpicker;

							if (draw_colorpicker)
								((UI_ColorPickerWindow*)App->gui->getUI_Element(COLOR_PICKER))->Draw(label.c_str(), (Color*)&c_mesh->getMesh()->tint_color, &draw_colorpicker, (Color*)&reference_color);
							else
								reference_color = c_mesh->getMesh()->tint_color;

							last_selected = c_mesh->getParent();
						}

						if (ImGui::Button("Add material"))
						{
							Material* mat = new Material();
							c_mesh->setMaterial(mat);
						}

					}
					ImGui::TreePop();
				}

			}

			if (ImGui::Button("Remove Component##Remove mesh"))
				ret = false;
		}
		break;
	case TRANSFORM:
		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::TextWrapped("Drag the parameters to change them, or ctrl+click on one of them to set it's value");
			ComponentTransform* c_trans = (ComponentTransform*)&component;

			static float3 position;
			static float3 rotation;
			static float3 scale;

			Transform* transform = nullptr;

			if (c_trans->getMode() == LOCAL)
			{
				transform = c_trans->local;
				ImGui::Text("Current mode: Local");
				ImGui::SameLine();
				if (ImGui::Button("Global"))
					c_trans->setMode(GLOBAL);
			}
			else
			{
				transform = c_trans->global;
				ImGui::Text("Current mode: Global");
				ImGui::SameLine();
				if (ImGui::Button("Local"))
					c_trans->setMode(LOCAL);
			}

			position = transform->getPosition();
			rotation = transform->getRotationEuler();
			scale = transform->getScale();

			//position
			ImGui::Text("Position:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[0][0])		ImGui::DragFloat("##p x", &position.x, 0.01f, 0.0f, 0.0f, "%.02f");
			else								ImGui::Text("%.2f", position.x);

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[0][1])	ImGui::DragFloat("##p y", &position.y, 0.01f, 0.0f, 0.0f, "%.02f");
			else								ImGui::Text("%.2f", position.y);

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[0][2])	ImGui::DragFloat("##p z", &position.z, 0.01f, 0.0f, 0.0f, "%.02f");
			else								ImGui::Text("%.2f", position.z);

			//rotation
			ImGui::Text("Rotation:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[1][0])	ImGui::DragFloat("##r x", &rotation.x, 0.2f, -180.0f, 180.0f, "%.02f");
			else								ImGui::Text("%.2f", rotation.z);

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[1][1])	ImGui::DragFloat("##r y", &rotation.y, 0.2f, -180.0f, 180.0f, "%.02f");
			else								ImGui::Text("%.2f", rotation.x);

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[1][2])	ImGui::DragFloat("##r z", &rotation.z, 0.2f, -180.0f, 180.0f, "%.02f");
			else								ImGui::Text("%.2f", rotation.y);

			//scale
			ImGui::Text("   Scale:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[2][0])	ImGui::DragFloat("##s x", &scale.x, 0.01f, -1000.0f, 1000.0f, "%.02f");
			else								ImGui::Text("%.2f", scale.x);

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[2][1])	ImGui::DragFloat("##s y", &scale.y, 0.01f, -1000.0f, 1000.0f, "%.02f");
			else								ImGui::Text("%.2f", scale.y);

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[2][2])	ImGui::DragFloat("##s z", &scale.z, 0.01f, -1000.0f, 1000.0f, "%.02f");
			else								ImGui::Text("%.2f", scale.z);

			if (ImGui::Button("Reset Transform"))
			{
				position = float3::zero; rotation = float3::zero, scale = float3::one;
			}

			ImGui::Checkbox("Draw axis", &c_trans->draw_axis);

			if (ImGui::CollapsingHeader("	Constraints"))
			{
				ImGui::Text("Position:");
				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint p x", &c_trans->constraints[0][0]);

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint p y", &c_trans->constraints[0][1]);

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint p z", &c_trans->constraints[0][2]);


				ImGui::Text("Rotation:");
				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint r x", &c_trans->constraints[1][0]);

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint r y", &c_trans->constraints[1][1]);

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint r z", &c_trans->constraints[1][2]);


				ImGui::Text("   Scale:");
				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint s x", &c_trans->constraints[2][0]);

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint s y", &c_trans->constraints[2][1]);

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint s z", &c_trans->constraints[2][2]);
			}

			transform->setPosition(position);
			transform->setRotationEuler(rotation);
			transform->setScale(scale);
		}
		break;

	case C_AABB:
		if (ImGui::CollapsingHeader("AABB"))
		{
			ComponentAABB* aabb = (ComponentAABB*)&component;

			static bool aabb_active;
			aabb_active = aabb->isActive();

			if (ImGui::Checkbox("Active##active AABB", &aabb_active))
				aabb->setActive(aabb_active);

			if (aabb_active)
			{
				static bool aabb_drawn;
				aabb_drawn = aabb->draw_aabb;

				if (ImGui::Checkbox("draw AABB", &aabb_drawn))
					aabb->draw_aabb = aabb_drawn;

				static bool obb_drawn;
				obb_drawn = aabb->draw_obb;

				ImGui::SameLine();
				if (ImGui::Checkbox("draw OBB", &obb_drawn))
					aabb->draw_obb = obb_drawn;

				if (ImGui::Button("Reload##Reload AABB"))
					aabb->Reload();
			}

		}
		break;
	case CAMERA:

		camera = (ComponentCamera*)&component;

		if (ImGui::CollapsingHeader("Camera"))
		{
			static bool camera_active;
			camera_active = camera->isActive();

			if (ImGui::Checkbox("Active##active camera", &camera_active))
				camera->setActive(camera_active);

			ImGui::Checkbox("Draw camera view", &camera->getCamera()->draw_in_UI);

			ImGui::Checkbox("Draw frustum", &camera->getCamera()->draw_frustum);

			ImGui::Checkbox("Draw depth", &camera->getCamera()->draw_depth);

			static bool overriding;
			overriding = (camera->getCamera() == App->camera->override_editor_cam_culling);
			if (ImGui::Checkbox("Override Frustum Culling", &overriding))
			{
				if (!overriding)	App->camera->override_editor_cam_culling = nullptr;
				else				App->camera->override_editor_cam_culling = camera->getCamera();
			}

			if (camera_active)
			{
				static float3 offset;
				offset = camera->offset;
				ImGui::Text("Offset:");
				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::DragFloat("##o x", &offset.x, 0.01f, -1000.0f, 1000.0f, "%.02f");

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::DragFloat("##o y", &offset.y, 0.01f, -1000.0f, 1000.0f, "%.02f");

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::DragFloat("##o z", &offset.z, 0.01f, -1000.0f, 1000.0f, "%.02f");

				camera->offset = offset;
			}

			if (ImGui::Button("Remove##Remove camera"))
				ret = false;
		}
		break;
	case SCRIPT:
	{
		ComponentScript* c_script = (ComponentScript*)&component;
		std::string component_title = c_script->script_name + "(Script)";
		if (ImGui::CollapsingHeader(component_title.c_str())) {

			if (!c_script->instance_data) {
				ImGui::Image((void*)App->gui->ui_textures[WARNING_ICON]->getGLid(), ImVec2(16, 16));
				ImGui::SameLine();
				ImGui::Text("Compile error");
			}
			else {
				for (auto it = c_script->instance_data->vars.begin(); it != c_script->instance_data->vars.end(); it++) {

					if (!(*it).isPublic())
						continue;

					ImportedVariable* curr = &(*it);
					std::string unique_tag = "##" + curr->getName();

					static int type = 0;

					if (!curr->isTypeForced())
					{
						type = curr->getType() - 1;
						if (ImGui::Combo(unique_tag.c_str(), &type, "Bool\0String\0Numeral\0"))
						{
							curr->setType((ImportedVariable::WrenDataType)(type + 1));
							Var nuller;
							switch (curr->getType())
							{
							case ImportedVariable::WrenDataType::WREN_BOOL:
								nuller.value_bool = false;
								break;
							case ImportedVariable::WrenDataType::WREN_NUMBER:
								nuller.value_number = 0;
								break;
							case ImportedVariable::WrenDataType::WREN_STRING:
								curr->value_string = "";
								break;
							}
							curr->SetValue(nuller);
							curr->setEdited(true);
						}
					}

					ImGui::Text(curr->getName().c_str());
					ImGui::SameLine();

					static char buf[200] = "";
					Var variable = curr->GetValue();

					switch (curr->getType()) {
					case ImportedVariable::WREN_NUMBER:
						if (ImGui::InputFloat((unique_tag + " float").c_str(), &variable.value_number))
						{
							curr->SetValue(variable);
							curr->setEdited(true);
						}
						break;
					case ImportedVariable::WREN_STRING:
					{
						strcpy(buf, curr->value_string.c_str());

						if (ImGui::InputText((unique_tag + " string").c_str(), buf, sizeof(buf)))
						{
							curr->value_string = buf;
							curr->SetValue(variable);
							curr->setEdited(true);
						}
					}
					break;
					case ImportedVariable::WREN_BOOL:
						if (ImGui::Checkbox((unique_tag + " bool").c_str(), &variable.value_bool))
						{
							curr->SetValue(variable);
							curr->setEdited(true);
						}
						break;
					}

				}
			}

			if (ResourceScript* res_script = (ResourceScript*)App->resources->getResource(c_script->getResourceUUID())) { // Check if resource exists
				if (ImGui::Button("Edit script")) {
					App->gui->ActivateUI_Element(SCRIPT_EDITOR);
					App->gui->open_script_path = res_script->asset;

					if (App->scripting->edited_scripts.find(App->gui->open_script_path) != App->scripting->edited_scripts.end())
						App->gui->script_editor.SetText(App->scripting->edited_scripts.at(App->gui->open_script_path));
					else {
						std::ifstream t(App->gui->open_script_path.c_str());
						if (t.good()) {
							std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
							App->gui->script_editor.SetText(str);
						}
					}
				}
			}
			if (ImGui::Button("Remove##Remove script"))
				ret = false;

		}

	}
	break;
	case AUDIOLISTENER:
		if (ImGui::CollapsingHeader("Audio Listener"))
		{
			if (ImGui::Checkbox("Mute", &App->audio->muted))
			{
				App->audio->SetVolume(App->audio->volume);
			}
			if (ImGui::SliderInt("Volume", &App->audio->volume, 0, 100))
			{
				App->audio->muted = false;
				App->audio->SetVolume(App->audio->volume);
			}
			if (ImGui::Button("Remove##Remove audioListener"))
				ret = false;
		}
		break;

	case AUDIOSOURCE:
		if (ImGui::CollapsingHeader("Audio Source"))
		{
			AkUniqueID ID = ((ComponentAudioSource*)&component)->sound_ID;
			if (ID != 0)
			{
				ImGui::TextColored({ 0, 1, 0, 1 }, ((ComponentAudioSource*)&component)->name.c_str());

				if (ImGui::Button("Play"))
				{
					((ComponentAudioSource*)&component)->sound_go->PlayEvent(ID);
				}
				ImGui::SameLine();
				if (ImGui::Button("Stop"))
				{
					((ComponentAudioSource*)&component)->sound_go->StopEvent(ID);
				}
				ImGui::SameLine();
				if (ImGui::Button("Pause"))
				{
					((ComponentAudioSource*)&component)->sound_go->PauseEvent(ID);
				}
				ImGui::SameLine();
				if (ImGui::Button("Resume"))
				{
					((ComponentAudioSource*)&component)->sound_go->ResumeEvent(ID);
				}
				if (ImGui::SliderInt("Volume", &((ComponentAudioSource*)&component)->volume, 0, 100))
				{
					App->audio->SetVolume(((ComponentAudioSource*)&component)->volume, ((ComponentAudioSource*)&component)->sound_go->GetID());
				}
				if (ImGui::SliderInt("Pitch", &((ComponentAudioSource*)&component)->pitch, -100, 100))
				{
					App->audio->SetPitch(((ComponentAudioSource*)&component)->pitch, ((ComponentAudioSource*)&component)->sound_go->GetID());
				}
			}
			else
			{
				ImGui::TextColored({ 1, 0, 0, 1 }, "No Audio Event assigned!");
			}
			if (ImGui::Button("Remove##Remove audioSource"))
				ret = false;
		}
		break;
	case CANVAS:
		if (ImGui::CollapsingHeader("Canvas"))
		{
			ComponentCanvas* canvas = (ComponentCanvas*)&component;
			ImGui::Text("Resolution  %.0f x %.0f", canvas->getResolution().x, canvas->getResolution().y);
			ImGui::Checkbox("Draw cross hair", &canvas->draw_cross);
		}
		break;
	case RECTTRANSFORM:
		if (ImGui::CollapsingHeader("Rect Transform"))
		{
			ComponentRectTransform* rectTrans = (ComponentRectTransform*)&component;

			static float2 position;
			static float width;
			static float height;

			position = rectTrans->getLocalPos();
			width = rectTrans->getWidth();
			height = rectTrans->getHeight();

			//position
			ImGui::Text("Position:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (ImGui::DragFloat2("##p", (float*)&position, 0.01f)) { rectTrans->setPos(position); }
			//Width
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (ImGui::DragFloat("##h", &width, 0.01f, 0.0f, 0.0f, "%.02f")) { rectTrans->setWidth(width); }
			//Height
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (ImGui::DragFloat("##w", &height, 0.01f, 0.0f, 0.0f, "%.02f")) {
				rectTrans->setHeight(height);
			}




			ImGui::Checkbox("Debug draw", &rectTrans->debug_draw);
		}
		break;
	case UI_IMAGE:
		if (ImGui::CollapsingHeader("UI Image"))
		{
			ComponentImageUI* image = (ComponentImageUI*)&component;

			ImGui::Image(image->getResourceTexture() != nullptr ? (void*)image->getResourceTexture()->texture->getGLid() : (void*)App->gui->ui_textures[NO_TEXTURE]->getGLid(), ImVec2(128, 128));
			ImGui::SameLine();

			int w = 0; int h = 0;
			if (image->getResourceTexture() != nullptr) {
				image->getResourceTexture()->texture->getSize(w, h);
			}

			ImGui::Text("texture data: \n x: %d\n y: %d", w, h);

			ImGui::SliderFloat("Alpha", &image->alpha, 0.0f, 1.0f);

			if (ImGui::Button("Load(from asset folder)##Dif: Load"))
			{
				std::string texture_path = openFileWID();
				uint new_resource = App->resources->getResourceUuid(texture_path.c_str());
				if (new_resource != 0) {
					App->resources->assignResource(new_resource);
					if (image->getResourceTexture() != nullptr)
						App->resources->deasignResource(image->getResourceTexture()->uuid);
					image->setResourceTexture((ResourceTexture*)App->resources->getResource(new_resource));
				}
			}
		}
		break;
	case UI_CHECKBOX:
		if (ImGui::CollapsingHeader("UI CheckBox"))
		{
			ComponentCheckBoxUI* chBox = (ComponentCheckBoxUI*)&component;

			ImGui::Image(chBox->getResourceTexture(CH_IDLE) != nullptr ? (void*)chBox->getResourceTexture(CH_IDLE)->texture->getGLid() : (void*)App->gui->ui_textures[NO_TEXTURE]->getGLid(), ImVec2(128, 128));
			ImGui::SameLine();

			int w = 0; int h = 0;
			if (chBox->getResourceTexture(CH_IDLE) != nullptr) {
				chBox->getResourceTexture(CH_IDLE)->texture->getSize(w, h);
			}

			ImGui::Text("Idle texture data: \n x: %d\n y: %d", w, h);

			if (ImGui::Button("Load(from asset folder)##Dif: Load"))
			{
				std::string texture_path = openFileWID();
				uint new_resource = App->resources->getResourceUuid(texture_path.c_str());
				if (new_resource != 0) {
					App->resources->assignResource(new_resource);
					if (chBox->getResourceTexture(CH_IDLE) != nullptr)
						App->resources->deasignResource(chBox->getResourceTexture(CH_IDLE)->uuid);
					chBox->setResourceTexture((ResourceTexture*)App->resources->getResource(new_resource), CH_IDLE);
				}
			}
			ImGui::Image(chBox->getResourceTexture(CH_PRESSED) != nullptr ? (void*)chBox->getResourceTexture(CH_PRESSED)->texture->getGLid() : (void*)App->gui->ui_textures[NO_TEXTURE]->getGLid(), ImVec2(128, 128));
			ImGui::SameLine();

			int w2 = 0; int h2 = 0;
			if (chBox->getResourceTexture(CH_PRESSED) != nullptr) {
				chBox->getResourceTexture(CH_PRESSED)->texture->getSize(w2, h2);
			}

			ImGui::Text("Pressed texture data: \n x: %d\n y: %d", w2, h2);

			if (ImGui::Button("Load(from asset folder)##Dif: Load2"))
			{
				std::string texture_path = openFileWID();
				uint new_resource = App->resources->getResourceUuid(texture_path.c_str());
				if (new_resource != 0) {
					App->resources->assignResource(new_resource);
					if (chBox->getResourceTexture(CH_PRESSED) != nullptr)
						App->resources->deasignResource(chBox->getResourceTexture(CH_PRESSED)->uuid);
					chBox->setResourceTexture((ResourceTexture*)App->resources->getResource(new_resource), CH_PRESSED);
				}
			} // For debug
			bool pressed = chBox->isPressed();
			ImGui::Checkbox("Pressed", &pressed);
			if (pressed != chBox->isPressed()) { chBox->Press(); }
		}
		break;
	case UI_TEXT:
		if (ImGui::CollapsingHeader("UI Text"))
		{
			ComponentTextUI* text = (ComponentTextUI*)&component;

			static const int maxSize = 32;
			if (ImGui::InputText("Label Text", (char*)text->label.text.c_str(), maxSize)) {
				text->SetText(text->label.text.c_str());
			}
			if (ImGui::SliderFloat("Scale", &(text->label.font->scale), 8, MAX_CHARS, "%0.f")) {
				text->SetFontScale(text->label.font->scale);
			}
			ImGui::Checkbox("Draw Characters Frame", &text->drawCharPanel);
			ImGui::Checkbox("Draw Label Frame", &text->drawLabelrect);
			std::string currentFont = text->label.font->fontSrc;
			if (ImGui::BeginCombo("Fonts", currentFont.c_str()))
			{
				std::vector<std::string> fonts = App->fontManager->singleFonts;

				for (int i = 0; i < fonts.size(); i++)
				{
					bool isSelected = false;

					if (strcmp(currentFont.c_str(), fonts[i].c_str()) == 0) {
						isSelected = true;
					}

					if (ImGui::Selectable(fonts[i].c_str(), isSelected)) {
						std::string newFontName = std::string(fonts[i].c_str());
						std::string newFontExtension = std::string(fonts[i].c_str());
						App->fs.getFileNameFromPath(newFontName);
						App->fs.getExtension(newFontExtension);
						newFontName += newFontExtension;
						text->SetFont(newFontName.c_str());

						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}

					}

				}
				ImGui::EndCombo();

			}
			ImGui::Spacing();
			ImGui::ColorPicker3("Color##2f", (float*)&text->label.color);
		}
		break;
	case UI_BUTTON:
		if (ImGui::CollapsingHeader("UI Button"))
		{
			ComponentButtonUI* button = (ComponentButtonUI*)&component;
			ButtonState state;//debug
			ImGui::Image(button->getResourceTexture(B_IDLE) != nullptr ? (void*)button->getResourceTexture(B_IDLE)->texture->getGLid() : (void*)App->gui->ui_textures[NO_TEXTURE]->getGLid(), ImVec2(128, 128));
			ImGui::SameLine();

			int w = 0; int h = 0;
			if (button->getResourceTexture(B_IDLE) != nullptr) {
				button->getResourceTexture(B_IDLE)->texture->getSize(w, h);
			}

			ImGui::Text("Idle texture data: \n x: %d\n y: %d", w, h);

			if (ImGui::Button("Load(from asset folder)##Dif: Load"))
			{
				std::string texture_path = openFileWID();
				uint new_resource = App->resources->getResourceUuid(texture_path.c_str());
				if (new_resource != 0) {
					App->resources->assignResource(new_resource);
					if (button->getResourceTexture(B_IDLE) != nullptr)
						App->resources->deasignResource(button->getResourceTexture(B_IDLE)->uuid);
					button->setResourceTexture((ResourceTexture*)App->resources->getResource(new_resource), B_IDLE);
				}
			}

			ImGui::Image(button->getResourceTexture(B_MOUSEOVER) != nullptr ? (void*)button->getResourceTexture(B_MOUSEOVER)->texture->getGLid() : (void*)App->gui->ui_textures[NO_TEXTURE]->getGLid(), ImVec2(128, 128));
			ImGui::SameLine();

			int w3 = 0; int h3 = 0;
			if (button->getResourceTexture(B_MOUSEOVER) != nullptr) {
				button->getResourceTexture(B_MOUSEOVER)->texture->getSize(w3, h3);
			}

			ImGui::Text("Hover texture data: \n x: %d\n y: %d", w3, h3);

			if (ImGui::Button("Load(from asset folder)##Dif: Load2"))
			{
				std::string texture_path = openFileWID();
				uint new_resource = App->resources->getResourceUuid(texture_path.c_str());
				if (new_resource != 0) {
					App->resources->assignResource(new_resource);
					if (button->getResourceTexture(B_MOUSEOVER) != nullptr)
						App->resources->deasignResource(button->getResourceTexture(B_MOUSEOVER)->uuid);
					button->setResourceTexture((ResourceTexture*)App->resources->getResource(new_resource), B_MOUSEOVER);
				}
			}


			ImGui::Image(button->getResourceTexture(B_PRESSED) != nullptr ? (void*)button->getResourceTexture(B_PRESSED)->texture->getGLid() : (void*)App->gui->ui_textures[NO_TEXTURE]->getGLid(), ImVec2(128, 128));
			ImGui::SameLine();

			int w2 = 0; int h2 = 0;
			if (button->getResourceTexture(B_PRESSED) != nullptr) {
				button->getResourceTexture(B_PRESSED)->texture->getSize(w2, h2);
			}

			ImGui::Text("Pressed texture data: \n x: %d\n y: %d", w2, h2);

			if (ImGui::Button("Load(from asset folder)##Dif: Load3"))
			{
				std::string texture_path = openFileWID();
				uint new_resource = App->resources->getResourceUuid(texture_path.c_str());
				if (new_resource != 0) {
					App->resources->assignResource(new_resource);
					if (button->getResourceTexture(B_PRESSED) != nullptr)
						App->resources->deasignResource(button->getResourceTexture(B_PRESSED)->uuid);
					button->setResourceTexture((ResourceTexture*)App->resources->getResource(new_resource), B_PRESSED);
				}
			}
			// For debug
			bool idle = false;
			bool hover = false;
			bool pressed = false;
			ImGui::Separator();
			if (ImGui::Button("FadeIn")) {
				button->doFadeIn();
			}
			ImGui::SameLine();
			if (ImGui::Button("FadeOut")) {
				button->doFadeOut();
			}
			if (ImGui::Button("Idle")) { button->setState(B_IDLE); } ImGui::SameLine();
			if (ImGui::Button("Hover")) { button->setState(B_MOUSEOVER); }ImGui::SameLine();
			if (ImGui::Button("Pressed")) { button->setState(B_PRESSED); }
		}
		break;
	case ANIMATION:
		if (ImGui::CollapsingHeader("Animation"))
		{
			//SKELETAL_TODO: missing resource info

			ComponentAnimation* anim = (ComponentAnimation*)&component;

			static bool animation_active;
			animation_active = anim->isActive();

			if (ImGui::Checkbox("Active##active animation", &animation_active))
				anim->setActive(animation_active);

			//Change/Add animation button
			//getAnimationResourceList()

			ImGui::Checkbox("Loop", &anim->loop);

			ImGui::PushID("Speed");
			ImGui::InputFloat("", &anim->speed, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopID();
		}
		break;
	case BONE:
		if (ImGui::CollapsingHeader("Bone"))
		{

		}
		break;
	default:
		break;
	}
	ImGui::PopID();

}