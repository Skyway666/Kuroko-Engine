#include "UI_HardwareLeaf.h"

#include "glew-2.1.0\include\GL\glew.h"
#include "VRAM.h"

void UI_HardwareLeaf::UpdateContent()
{

	//CPUs
	ImGui::Text("CPUs");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%i", SDL_GetCPUCount());

	// RAM
	ImGui::Text("System RAM");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%i Gb", SDL_GetSystemRAM());

	// Caps
	ImGui::Text("Caps:");
	ImGui::SameLine();
	if (SDL_HasRDTSC())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "RDTSC, ");
	ImGui::SameLine();
	if (SDL_HasMMX())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "MMX, ");
	ImGui::SameLine();
	if (SDL_HasSSE())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "SSE, ");
	ImGui::SameLine();
	if (SDL_HasSSE2())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "SSE2, ");
	if (SDL_HasSSE3())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "SSE3, ");
	ImGui::SameLine();
	if (SDL_HasSSE41())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "SSE41, ");
	ImGui::SameLine();
	if (SDL_HasSSE42())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "SSE42, ");
	ImGui::SameLine();
	if (SDL_HasAVX())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "AVX.");
	ImGui::SameLine();

	ImGui::Separator();
	//GPU
	ImGui::Text("Caps:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%s", glGetString(GL_VENDOR));
	ImGui::Text("Brand:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%s", glGetString(GL_RENDERER));
	ImGui::Text("VRAM Budget:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%f Mb", getTotalVRAMMb_NVIDIA());
	ImGui::Text("VRAM Usage:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%f Mb", getTotalVRAMMb_NVIDIA() - getAvaliableVRAMMb_NVIDIA());
	ImGui::Text("VRAM Avaliable:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%f Mb", getAvaliableVRAMMb_NVIDIA());
	ImGui::Text("VRAM Reserved:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%f Mb", 0);
}