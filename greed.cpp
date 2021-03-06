/*
 * @Author       : Chivier Humber
 * @Date         : 2021-08-11 22:52:25
 * @LastEditors  : Chivier Humber
 * @LastEditTime : 2021-08-12 17:17:15
 * @Description  : file content
 */
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <array>
#include <algorithm>
#include <limits>
#include <cstddef>

using namespace std;

const int kMaxM = 1000;
const int kMaxN = 1000000;

const int kInterval = 0;

int gN;
int gM, gm;
int gFactor;
std::array<int, kMaxN> gTask;
std::array<int, kMaxM> gResourcesPlan;
std::array<int, kMaxN> gPlan; // Resource Amount
std::array<int, kMaxN> gTime; // Start time

int gcd(int a, int b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

int lcm(int a, int b) {
    return a * b / gcd(a, b);
}

inline int TimeEstimate(int u, int v) {
    return (u - 1) / v + 1;
}


/**
 * @description: Read Config information
 * @param {*}
 * @return {*}
 */
void ReadConfig() {
    ifstream file("config.txt");
    if (file.is_open()) {
        file >> gM;
        file >> gm;
        for (int index = 0; index < gm; ++index) {
            file >> gResourcesPlan[index];
        }
    }
    file.close();
    sort(gResourcesPlan.begin(), gResourcesPlan.begin() + gm, std::greater<int>());
    // gFactor = gResourcesPlan[0];
    // for (int index = 1; index < gm; ++index) {
    //     gFactor = lcm(gFactor, gResourcesPlan[index]);
    // }
}

/**
 * @description: Read Task information
 * @param {*}
 * @return {*}
 */
void ReadTask() {
    ifstream file("file.txt");
    if (file.is_open()) {
        file >> gN;
        for (int index = 0; index < gN; ++index) {
            file >> gTask[index];
            // Promise all the time interval is integer
            // gTask[index] *= gFactor;
        }
    }
    file.close();
    sort(gTask.begin(), gTask.begin() + gN, std::greater<int>());
}

/**
 * @description: Split the resources in a greed method
 * @param {*}
 * @return {*}
 */
std::vector<int> SplitResource() {
    std:;vector<int> split;
    int resource_total = gM;
    for (int index = 0; index < gm; ++index) {
        int split_times = resource_total / gResourcesPlan[index];
        resource_total = resource_total % gResourcesPlan[index];

        for (int sub_index = 0; sub_index < split_times; ++sub_index) {
            split.push_back(gResourcesPlan[index]);
        }

        if (resource_total == 0) {
            // all the resources are used
            break;
        }
    }

    cout << "Intrinsic Plan:" << endl;
    for (auto const &num : split) {
        cout << num << " ";
    }
    cout << endl;

    return split;
}

void PrintTags(vector<int> const &tags, bool detailed=false) {
    if (detailed) {
        for (auto item : tags) {
            cout << item << " ";
        }
        cout << endl;
    } else {
        int tag = 0;
        for (auto item : tags) {
            if (item == -1)
                tag ++;
        }
        cout << "resources = " << tag << endl;
    }
}

/**
 * @description: Arrange all the tasks
 * @param {vector<int>} split
 * @return {*}
 */
void Arrange(std::vector<int> split) {
    int split_size = split.size();
    double usage_rate = 0;
    vector<int> time_list(split_size, 0);
    vector<int> arrange_tag(gM, -1);

    int total_time = 0;
    // Give a guess on the next task

    // First tuen is the same as intrinsic method
    int start_tag_position = 0;
    for (int index = 0; index < min(split_size, gN); ++index) {
        time_list[index] = TimeEstimate(gTask[index], split[index]) + kInterval;
        usage_rate += TimeEstimate(gTask[index], split[index]) * split[index] - gTask[index];
        gTime[index] = 0;
        gPlan[index] = split[index];

        total_time += TimeEstimate(gTask[index], split[index]);
        // Place the tag on the corresponding places
        for (int sub_index = start_tag_position; sub_index < start_tag_position + split[index]; ++sub_index) {
            arrange_tag[sub_index] = index;
        }

        start_tag_position += split[index];
    }

    // PrintTags(arrange_tag);
    int index = min(split_size, gN);
    while (index < gN) {
        // If there is already resources available
        int empty_resources = 0;

        // Find the earliest ended task
        int time_select = std::numeric_limits<int>::max();
        int flag_index = -1;
        
        for (int sub_index = 0; sub_index < gM; ++sub_index) {
            int tag = arrange_tag[sub_index];
            if (tag == -1) {
                continue;
            }
            int end_time = gTime[tag] + TimeEstimate(gTask[tag], gPlan[tag]);
            if (end_time < time_select) {
                time_select = end_time;
                flag_index = tag;
            }
        }
        
        // Arrange the index-th task
        empty_resources = 0;
        for (int sub_index = 0; sub_index < gM; ++sub_index) {
            int tag = arrange_tag[sub_index];
            if (tag == -1) {
                empty_resources++;
                continue;
            }
            if (gTime[tag] + TimeEstimate(gTask[tag], gPlan[tag]) == time_select) {
                arrange_tag[sub_index] = -1;
                empty_resources++;
            }
        }
        PrintTags(arrange_tag);
        while (empty_resources) {
            // Use all the empty_resources
            // Guess the resources for the next task
            double guess_time = ((double) total_time) / (index - 1);
            double guess_resource = gTask[index] / guess_time;
            cout << "Deal : " << index << " amount = " << gTask[index] << "\n";
            cout << "Guessing for " << guess_resource << endl;
            int resources_next = 0;
            double guess_distance = std::numeric_limits<double>::max();
            for (int resource_index = gm - 1; resource_index >= 0; --resource_index) {
                // Promise the usage of resources is legal
                if (fabs(gResourcesPlan[resource_index] - guess_resource) <= guess_distance &&
                gResourcesPlan[resource_index] <= empty_resources) {
                    guess_distance = fabs(gResourcesPlan[resource_index] - guess_resource);
                    resources_next = gResourcesPlan[resource_index];
                }
            }
            
            if (resources_next == 0) {
                resources_next = gResourcesPlan[0];
            }

            total_time += TimeEstimate(gTask[index], resources_next);
            usage_rate += TimeEstimate(gTask[index], resources_next) * resources_next - gTask[index];
            empty_resources -= resources_next;
            gTime[index] = time_select;
            gPlan[index] = resources_next;

            cout << "Give for " << resources_next << endl << endl;
            int tag_count = 0;
            for (int sub_index = 0; sub_index < gM; ++sub_index) {
                if (arrange_tag[sub_index] == -1) {
                    arrange_tag[sub_index] = index;
                    tag_count++;
                }
                if (tag_count == resources_next)
                    break;
            }
            index++;

            if (index >= gN) {
                break;
            }
        }
    }

    int final_time = 0;
    int begin_time = std::numeric_limits<int>::max();
    int final_index = -1;
    
    for (int index = 0; index < gM; ++index) {
        int tag = arrange_tag[index];
        if (tag == -1) {
            continue;
        }
        int end_time = gTime[tag] + TimeEstimate(gTask[tag], gPlan[tag]);
        if (end_time > final_time) {
            final_time = end_time;
            final_index = tag;
        }
        if (end_time < begin_time) {
            begin_time = end_time;
        }
    }

    for (int index = 0; index < gM; ++index) {
        int tag = arrange_tag[index];
        if (tag == -1) {
            usage_rate += (final_time - begin_time);
            continue;
        }
        int end_time = gTime[tag] + TimeEstimate(gTask[tag], gPlan[tag]);
        usage_rate += (final_time - end_time);
    }


    usage_rate = 1 - usage_rate / (gM * final_time);
    usage_rate *= 100;
    
    cout << "Plan :" << endl;
    for (int index = 0; index < gN; ++index) {
        cout << index << " task = "
        << gTask[index] << " begin time = "<< gTime[index] << "   resources = " << gPlan[index] << endl;
    }
    cout << "Final time = " << final_time << " final index = " << final_index << endl;
    cout << "Usage rate = " << usage_rate << " %" << endl;
}

int main() {
    ReadConfig();
    ReadTask();
    auto split = SplitResource();
    Arrange(split);
    return 0;
}