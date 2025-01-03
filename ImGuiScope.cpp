#include "ImGuiScope.hpp"

#include <imgui.h>

#include <iostream>

void ImGuiScope::putResult(const std::string& timerName, const TimerResult& result)
{
	if (!ImGuiScope::Internal::timerMap.contains(timerName))
	{
		ImGuiScope::Internal::timerMap.emplace(timerName, std::make_unique<TimerResultBuffer>());
	}
	ImGuiScope::Internal::timerMap[timerName]->put(result);
}

void ImGuiScope::drawResultsHeader(std::string headerTitle)
{
	if (ImGui::CollapsingHeader(headerTitle.c_str())) 
	{
		ImGui::Indent();
		for (const auto& [timerName, timerBuffer] : ImGuiScope::Internal::timerMap)
		{
			if (ImGui::CollapsingHeader(timerName.c_str()))
			{
				int index = 0;
				ImGui::PushID(ImGui::GetID(timerName.c_str()));

				ImGui::TextUnformatted("Timespan:");
				for (const auto& timespan : TimerResultBuffer::timeBufferSizeNames)
				{
					ImGui::RadioButton(timespan, &timerBuffer->timeBufferSizeIndex, index);

					if (timespan != TimerResultBuffer::timeBufferSizeNames.back())
					{
						ImGui::SameLine();
					}
					index++;
				}

				index = 0;
				ImGui::TextUnformatted("Unit:");
				for (const auto& unitName : TimerResultBuffer::unitNames)
				{
					ImGui::RadioButton(unitName, &timerBuffer->unitNameIndex, index);
					if (unitName != TimerResultBuffer::unitNames.back())
					{
						ImGui::SameLine();
						index++;
					}
				}
				
				if (!timerBuffer->isActive) timerBuffer->isActive = true;
				ImGui::Value("Count", timerBuffer->count());
				ImGui::Value("Last", timerBuffer->last());
				std::cout << timerBuffer->last() << "\n";
				ImGui::Value("Mean", timerBuffer->mean());
				ImGui::PopID();
			}
			else if (timerBuffer->isActive) timerBuffer->isActive = false;
		}
		ImGui::Unindent();
	}
	else
	{
		for (const auto& [timerName, timerBuffer] : ImGuiScope::Internal::timerMap)
		{
			if (timerBuffer->isActive) timerBuffer->isActive = false;
		}
	}
}

ImGuiScope::TimeScope::TimeScope(const std::string& timerName, const bool& consoleOutput) :
	timerName_(timerName),
	consoleOutput_(consoleOutput)
{
	if (!ImGuiScope::Internal::timerMap.contains(timerName_))
	{
		ImGuiScope::Internal::timerMap.emplace(timerName, std::make_unique<TimerResultBuffer>());
	}
}

ImGuiScope::TimeScope::~TimeScope()
{
	if (!ImGuiScope::Internal::timerMap[timerName_]->isActive) return;

	TimerResult result
	{
		startTime_,
		std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - startTime_)
	};

	ImGuiScope::putResult(timerName_, result);

	if (consoleOutput_) std::cout << "Timer " << timerName_ << "timed out with " << result.elapsed.count() << "microseconds.\n";
}

	