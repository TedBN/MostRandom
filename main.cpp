#include <memory>
#include <random>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <array>
#include <thread>
#include <iomanip>
#include "ListFunctions.h"
#include "EventTriggerFunctions.h"
#include "GeneralFunctions.h"
#include "RaceFunctions.h"
#include "FreeRoamFunctions.h"
#include "BossRaceMarkerFunctions.h"
#include "NodeConversionFunctions.h"
#include "PursuitFunctions.h"

int getSizeOfChildrenFieldForChapters(const std::string& raceBin)
{
    if ( raceBin == "race_bin_15" ) { return 13; }
    else if ( raceBin == "race_bin_14" ) { return 15; }
    else if ( raceBin == "race_bin_13" || raceBin == "race_bin_08" ) { return 16; }
    else if ( raceBin == "race_bin_12" ) { return 17; }
    else if ( raceBin == "race_bin_11" || raceBin == "race_bin_10" || raceBin == "race_bin_09" ) { return 18; }
    else if ( raceBin == "race_bin_07" ) { return 20; }
    else if ( raceBin == "race_bin_06" || raceBin == "race_bin_05" ) { return 21; }
    else if ( raceBin == "race_bin_04" || raceBin == "race_bin_03" || raceBin == "race_bin_02" ) { return 22; }
    else { return 24; }
}

std::vector<std::string> updateNumberOfChapterRaces(std::ofstream& file, int numberOfRaces, int numberOfOriginalRaces,
                                int numberOfBossRaces, int numberOfOriginalBossRaces,
                                const std::string& raceBin, std::vector<std::string> bossIndexes)
{
    std::vector<int> possibleMinimumRaceWins;
    float numberOfRacesFloat;
    numberOfRacesFloat = (float)numberOfRaces;

    file << std::string("# changing number of races for ").append(raceBin).append("\n");

    if ( numberOfRaces != numberOfOriginalRaces || numberOfBossRaces != numberOfOriginalBossRaces
         || raceBin == "race_bin_15" )
    {
        if ( raceBin != "race_bin_15" )
        {
            file << std::string("resize_field gameplay ").append(raceBin).append(" Children ")
            .append(std::to_string((numberOfBossRaces - numberOfOriginalBossRaces) +
            (numberOfRaces - numberOfOriginalRaces) + getSizeOfChildrenFieldForChapters(raceBin)))
            .append("\n");
        }

        else
        {
            file << std::string("resize_field gameplay ").append(raceBin).append(" Children ")
            .append(std::to_string((numberOfBossRaces - numberOfOriginalBossRaces) +
            (numberOfRaces - numberOfOriginalRaces) + getSizeOfChildrenFieldForChapters(raceBin) + 3 ))
            .append("\n");
        }

        if ( numberOfRaces != numberOfOriginalRaces || raceBin == "race_bin_15" )
        {
            file << std::string("resize_field gameplay ").append(raceBin).append(" WorldRaces ")
            .append(std::to_string(numberOfRaces)).append("\n");
        }

        if ( numberOfBossRaces != numberOfOriginalBossRaces )
        {
            file << std::string("resize_field gameplay ").append(raceBin).append(" BossRaces ")
            .append(std::to_string(numberOfBossRaces)).append("\n");
        }

        int differenceBetweenWouldBeChildrenFieldSizeAndOriginalSize =
                (numberOfRaces - numberOfOriginalRaces) + (numberOfBossRaces - numberOfOriginalBossRaces);

        if ( differenceBetweenWouldBeChildrenFieldSizeAndOriginalSize > 0 )
        {
            for ( int loop = 0; loop < differenceBetweenWouldBeChildrenFieldSizeAndOriginalSize; loop++ )
            { bossIndexes.push_back(std::to_string(std::stoi(bossIndexes.back()) + 1)); }
        }

        else if ( differenceBetweenWouldBeChildrenFieldSizeAndOriginalSize < 0)
        {
            for ( int loop = 0; loop > differenceBetweenWouldBeChildrenFieldSizeAndOriginalSize; loop-- )
            { bossIndexes.erase(bossIndexes.begin() + (int)bossIndexes.size()); }
        }
    }

    if ( numberOfRaces != 1 )
    {
        for ( int loop = 0; loop < numberOfRaces; loop++ )
        {
            if ( ((float)loop + 1) / numberOfRacesFloat >= 0.20 && ((float)loop + 1) / numberOfRacesFloat <= 0.80 )
            { possibleMinimumRaceWins.emplace_back(loop + 1); }
        }
    }

    else { possibleMinimumRaceWins.emplace_back(1); }

    file << std::string("update_field gameplay ").append(raceBin).append(" RequiredRacesWon ")
    .append(std::to_string(possibleMinimumRaceWins[pickRandomNumber(0,
    (int)possibleMinimumRaceWins.size() - 1)])).append("\n\n");

    return bossIndexes;
}

void randomizeOpponents(const std::array<std::string, 41>& regularOpponentList,
                        const std::array<std::string, 18>& scriptedOpponentList,
                        const std::array<std::string, 6>& startingOpponentList,
                        const std::array<std::string, 35>& aiCarList)
{
    //Variables
    std::ofstream file;
    file.open("Random.nfsms", std::ios_base::app);
    int newSkillLevel;
    int averageSkillLevel = 30;
    double averageSkillLevelFloat = 30;
    std::vector<std::string> fullOpponentList;

    //These value are used for the scripted Razor opponents
    float prologueRazorPerformanceField = static_cast<float>(pickRandomNumber(0, 10))/10;
    int prologueRazorSkillLevelField = pickRandomNumber(0, 35);

    fullOpponentList.reserve(scriptedOpponentList.size());
    for ( const std::string& opponent : scriptedOpponentList ) { fullOpponentList.emplace_back(opponent); }

    fullOpponentList.reserve(regularOpponentList.size());
    for ( const std::string& opponent : regularOpponentList ) { fullOpponentList.emplace_back(opponent); }

    fullOpponentList.reserve(startingOpponentList.size());
    for ( const std::string& opponent : startingOpponentList ) { fullOpponentList.emplace_back(opponent); }

    file << std::string("# AI difficulty and cars for opponents are randomized here").append("\n");

    for ( int loop = 0; loop < fullOpponentList.size(); loop++ )
    {
        //Change vault to gpcore for non-scripted opponents, which is accessible from all races
        if ( loop < 3 || loop > 17 )
        {
            file << std::string("change_vault gameplay ").append(fullOpponentList[loop])
            .append(" gpcore").append("\n");
        }

        //Set skill level based on current average
        if ( averageSkillLevel < 35 ) { newSkillLevel = pickRandomNumber(0, 35 + averageSkillLevel ); }
        else if ( averageSkillLevel > 65 ) { newSkillLevel = pickRandomNumber(averageSkillLevel - 35, 100); }
        else { newSkillLevel = pickRandomNumber(averageSkillLevel - 35, averageSkillLevel + 35); }

        file << std::string("update_field gameplay ").append(fullOpponentList[loop]).append(" SkillLevel ")
        .append(std::to_string(newSkillLevel)).append("\n");

        //Add performance scaling field to opponents who don't have it
        if ( loop < 3 || (loop > 17 && loop < 20) )
        {
            file << std::string("add_field gameplay ").append(fullOpponentList[loop])
            .append(" 0xb1ece070").append("\n");
        }

        //All opponents except the starting ones have this field randomized
        if (  loop != 3 && loop != 4 && loop < 59 )
        {
            file << std::string("update_field gameplay ").append(fullOpponentList[loop]).append(" 0xb1ece070 ");
            file << std::setprecision(2) << static_cast<float>(pickRandomNumber(0, 10))/10;
            file << std::string("\n");
        }

        //All boss opponent cars remain preset, but the exact car is randomized
        if ( loop < 20 )
        {
            file << std::string("update_field gameplay ").append(fullOpponentList[loop])
            .append(" PresetRide ").append(aiCarList[pickRandomNumber(0, (int)aiCarList.size() - 1)])
            .append("\n");
        }

        //Pool of opponents for regular races have a set chance of using fixed cars
        else
        {
            if ( loop < 59 && pickRandomNumber(0, 100) < 50 )
            {
                file << std::string("add_field gameplay ").append(fullOpponentList[loop])
                .append(" PresetRide").append("\n");

                file << std::string("update_field gameplay ").append(fullOpponentList[loop])
                .append(" PresetRide ").append(aiCarList[pickRandomNumber(0, (int) aiCarList.size() - 1)])
                .append("\n");
            }
        }

        //Check for points in loop where skill should be increased or decreased
        if ( loop != 3 && loop != 17 && loop != 19 )
        {
            if ( loop < 20 )
            {
                averageSkillLevelFloat = averageSkillLevelFloat + 6.20;
                averageSkillLevel = (int)round(averageSkillLevelFloat);
            }

            else
            {
                averageSkillLevelFloat = averageSkillLevelFloat - 2.11;
                averageSkillLevel = (int)round(averageSkillLevelFloat);
            }
        }

        //Check for points where average skill should be reset
        else
        {
            if ( loop == 3 )
            {
                averageSkillLevelFloat = 0;
                averageSkillLevel = 0;
            }

            else if ( loop == 17 )
            {
                averageSkillLevelFloat = 42;
                averageSkillLevel = 42;
            }

            else
            {
                averageSkillLevelFloat = 100;
                averageSkillLevel = 100;
            }
        }
    }

    //Modify prologue Razor opponent values while maintaining consistency
    file << std::string("update_field gameplay 16_2_1_d_day_intro/opponent SkillLevel ")
    .append(std::to_string(prologueRazorSkillLevelField)).append("\n");

    file << std::string("add_field gameplay 16_2_1_d_day_intro/opponent 0xb1ece070").append("\n");

    file << std::string("update_field gameplay 16_2_1_d_day_intro/opponent 0xb1ece070 ")
    .append(std::to_string(prologueRazorPerformanceField)).append("\n");

    file << std::string("update_field gameplay 16_1_0_partial_dday/razor SkillLevel ")
    .append(std::to_string(prologueRazorSkillLevelField)).append("\n");

    file << std::string("add_field gameplay 16_1_0_partial_dday/razor 0xb1ece070").append("\n");

    file << std::string("update_field gameplay 16_1_0_partial_dday/razor 0xb1ece070 ")
    .append(std::to_string(prologueRazorPerformanceField)).append("\n");

    //Tweak a few unnecessary fields for prologue opponents
    file << std::string("delete_field gameplay 16_1_1_circuit/opponent ForceStartPosition").append("\n");
    file << std::string("delete_field gameplay 16_1_1_circuit/opponent Children").append("\n");
    file << std::string("delete_field gameplay 16_2_1_d_day_intro/opponent ForceStartPosition").append("\n");
    file << std::string("delete_field gameplay 16_2_2_sprint/ronnie_mccrae ForceStartPosition").append("\n");
    file << std::string("delete_field gameplay 16_2_2_sprint/ronnie_mccrae Children").append("\n");
    file << std::string("delete_field gameplay 16_2_3_sprint/toru_sato Children").append("\n\n");

    file.close();
}

std::vector<std::pair<std::string, bool>> randomizePrologueRaces(int numberOfRaces,
                          std::vector<std::pair<std::string, bool>> races, int raceCashValue,
                          const std::unordered_map<std::string, int>& raceMap,
                          const std::array<std::string, 41>& opponentList, std::array<std::string, 73> carList,
                          const std::unordered_map<int, float>& distanceMap,
                          const std::array<std::string, 35>& aiCarList)
{
    //Initial variables
    std::ofstream file;
    std::ofstream& fileReference = file;
    int pickedRace;
    std::unordered_map<std::string, int>::const_iterator raceInteger;
    std::string raceName;
    std::string fullRaceName;
    std::string raceType;
    std::string emptyPostRaceActivityField;
    std::string pickedMarker;
    int copChance = pickRandomNumber(0, 75);
    const std::string& pickedRazorCar = aiCarList[pickRandomNumber(0, (int)aiCarList.size() - 1)];
    std::string pickedPlayerCar = carList[pickRandomNumber(0, (int)carList.size() - 1)];
    const std::string& pickedRogCar = aiCarList[pickRandomNumber(0, (int)aiCarList.size() - 1)];
    std::array<std::string, 12> fmvs = getListOfFMVs();

    file.open("Random.nfsms", std::ios_base::app);

    //Prologue Razor races and final chase are set to the chosen car
    file << std::string("# set car for events not in the pool and randomize scripted opponents").append("\n");

    file << std::string("update_field gameplay 16_1_0_partial_dday PlayerCarType ")
    .append(pickedPlayerCar).append("\n");

    file << std::string("update_field gameplay 16_2_1_d_day_intro PlayerCarType ")
    .append(pickedPlayerCar).append("\n");

    file << std::string("add_field gameplay 16_1_0_partial_dday PlayerCarPerformance").append("\n");
    file << std::string("update_field gameplay 16_1_0_partial_dday PlayerCarPerformance 0.5").append("\n");

    file << std::string("add_field gameplay 16_2_1_d_day_intro PlayerCarPerformance").append("\n");
    file << std::string("update_field gameplay 16_2_1_d_day_intro PlayerCarPerformance 0.5").append("\n");

    file << std::string("update_field gameplay race_bin_01/epic_pursuit PlayerCarType ")
    .append(pickedPlayerCar).append("\n");

    file << std::string("add_field gameplay race_bin_01/epic_pursuit PlayerCarPerformance").append("\n");
    file << std::string("update_field gameplay race_bin_01/epic_pursuit PlayerCarPerformance 1").append("\n");

    //Use same car for all prologue Razor nodes and randomize Rogs car
    file << std::string("update_field gameplay 16_2_1_d_day_intro/opponent PresetRide ")
    .append(pickedRazorCar).append("\n");

    file << std::string("update_field gameplay 16_1_0_partial_dday/razor PresetRide ")
    .append(pickedRazorCar).append("\n");

    file << std::string("update_field gameplay 16_nis_rog_razor/razor PresetRide ")
    .append(pickedRazorCar).append("\n");

    file << std::string("update_field gameplay 16_nis_rog_razor/rog PresetRide ")
    .append(pickedRogCar).append("\n");

    file << std::string("update_field gameplay 16_nis_rog_razor/rog PresetRide ")
    .append(pickedRogCar).append("\n");

    file << std::string("update_field gameplay 16_1_1_circuit/opponent PresetRide ")
    .append(pickedRogCar).append("\n");

    file << std::string("update_field gameplay race_bin_01/razor_callahan PresetRide ")
    .append(pickedPlayerCar).append("\n\n");

    for ( int loop = 0; loop < numberOfRaces; loop++ )
    {
        //Writes a comment that says which race node was chosen and as which race in the text file
        file << std::string("# prologue, race ").append(std::to_string(loop + 1)).append("\n");

        //Pick race at random and retrieve its integer from raceMap
        pickedRace = pickRandomNumber(0, static_cast<int>(races.size()) - 1);
        raceName = races[pickedRace].first;
        raceInteger = raceMap.find(raceName);

        //Race type is used both for naming triggers and knowing which fields to randomize later
        raceType = checkRaceType(raceInteger->second);

        //Updates vault for race if originally from a blacklist member
        if ( raceName.substr(0, 2) != "16" )
        {
            convertToPrologueRace(fileReference, raceName, raceInteger->second, raceCashValue);

            if ( raceInteger->second < 172 )
            {
                //If vanilla race with trigger, delete it
                if ( hasTrigger(raceInteger->second) )
                {
                    std::vector<std::string> triggerDetails = getNewTriggerDetails(raceInteger->second);

                    file << std::string("delete_node gameplay ").append(raceName.substr(0, 11))
                    .append(triggerDetails[0]).append("\n");
                }
            }
        }

        else { removeUnnecessaryPrologueRaceFields(fileReference, raceName); }

        //Deal with PostRaceActivity fields and opponents depending on current cycle
        switch(loop)
        {
            case 0:
                file << std::string("update_field gameplay 16_1_0_partial_dday PostRaceActivity ")
                .append(raceName).append("\n");

                emptyPostRaceActivityField = raceName;

                if ( raceType != "tollbooth" )
                {
                    if ( !races[pickedRace].second )
                    {
                        file << std::string("resize_field gameplay ").append(raceName)
                        .append(" Opponents 1").append("\n");
                    }

                    file << std::string("update_field gameplay ").append(raceName)
                    .append(" Opponents[0] 16_2_2_sprint/ronnie_mccrae").append("\n");
                }

                break;

            case 1:
                file << std::string("update_field gameplay ").append(emptyPostRaceActivityField)
                .append(" PostRaceActivity ").append(raceName).append("\n");

                emptyPostRaceActivityField = raceName;

                if ( raceType != "tollbooth" )
                {
                    if ( !races[pickedRace].second && raceName != "16_2_3_sprint" && raceName != "16_2_2_sprint" )
                    {
                        file << std::string("resize_field gameplay ").append(raceName)
                        .append(" Opponents 1").append("\n");
                    }

                    file << std::string("update_field gameplay ").append(raceName)
                    .append(" Opponents[0] 16_2_3_sprint/toru_sato").append("\n");
                }

                break;

            default:
                file << std::string("update_field gameplay ").append(emptyPostRaceActivityField)
                .append(" PostRaceActivity ").append(raceName).append("\n");

                file << std::string("update_field gameplay ").append(raceName)
                .append(" PostRaceActivity 16_nis_rog_razor").append("\n");

                if ( raceType != "tollbooth" )
                {
                    if ( races[pickedRace].second || raceName == "16_2_3_sprint" || raceName == "16_2_2_sprint" )
                    {
                        file << std::string("resize_field gameplay ").append(raceName)
                        .append(" Opponents 3").append("\n");
                    }

                    for ( int opponentLoop = 0; opponentLoop < 2; opponentLoop++ )
                    {
                        file << std::string("update_field gameplay ").append(raceName).append(" Opponents[")
                        .append(std::to_string(opponentLoop)).append("] ")
                        .append(opponentList[pickRandomNumber(0, (int)opponentList.size() - 1)])
                        .append("\n");
                    }

                    file << std::string("update_field gameplay ").append(raceName)
                    .append(" Opponents[2] 16_1_1_circuit/opponent").append("\n");
                }
        }

        //FMVs are randomized here
        file << std::string("update_field gameplay ").append(raceName).append(" IntroMovie ")
        .append(fmvs[pickRandomNumber(0, (int)fmvs.size() - 1)]).append("\n");

        //Function responsible for intros is called for any non-tollbooth race
        if ( raceType != "tollbooth" && raceName != "16_1_1_circuit" )
        { 
			switch(loop)
            {
                case 0: case 1:
                    randomizeRaceIntro(fileReference, raceName, raceInteger->second, true);
                    break;

                default:
                    randomizeRaceIntro(fileReference, raceName, raceInteger->second, false);
            } 
		}

        randomizeGeneralFields(fileReference, raceName, distanceMap, numberOfRaces, raceType,
                               raceInteger->second, loop, copChance);

        //M3 is changed for a random preset car
        if ( !checkForField(raceInteger->second, 11) )
        { file << std::string("add_field gameplay ").append(raceName).append(" PlayerCarType").append("\n"); }

        file << std::string("update_field gameplay ").append(raceName)
        .append(" PlayerCarType ").append(pickedPlayerCar).append("\n");

        //Car performance is reduced in prologue
        file << std::string("add_field gameplay ").append(raceName).append(" PlayerCarPerformance")
        .append("\n");

        file << std::string("update_field gameplay ").append(raceName).append(" PlayerCarPerformance 0.5")
        .append("\n\n");
		
		//Check for a single Big Lou race that has a misspelled Name field and correct it
        if ( raceName == "race_bin_11/11_2_1_sprint" )
        { file << std::string("update_field gameplay race_bin_11/11_2_1_sprint Name race_bin_11/11_2_1_sprint\n"); }

        //Remove processed race from pool
        races.erase(races.begin() + pickedRace);
    }

    file << std::string("# FMVs for prologue Razor races").append("\n");

    //FMVs for races outside randomized ones are processed here
    file << std::string("update_field gameplay 16_1_0_partial_dday").append(" IntroMovie ")
    .append(fmvs[pickRandomNumber(0, (int)fmvs.size() - 1)]).append("\n");

    file << std::string("update_field gameplay 16_2_1_d_day_intro").append(" OutroMovie ")
    .append(fmvs[pickRandomNumber(0, (int)fmvs.size() - 1)]).append("\n\n");

    file.close();

    //All done
    return races;
}

std::vector<std::pair<std::string, bool>> randomizeRaces(std::ofstream& file,
                          int numberOfRaces, int numberOfBossRaces, int bossRaceCashValue, const std::string& raceBin,
                          const std::string& bossOpponentString, std::vector<std::pair<std::string, bool>> races,
                          int raceCashValue, const std::unordered_map<std::string, int>& raceMap,
                          const std::array<std::string, 41>& opponentList,
                          const std::vector<std::string>& bossIndexes,
                          const std::unordered_map<int, float>& distanceMap,
                          const std::array<std::string, 6>& startingOpponentList)
{
    //Initial variables
    int pickedRace;
    std::unordered_map<std::string, int>::const_iterator raceInteger;
    int bossRaceCounter = 0;
    std::string raceName;
    std::string raceType;
    std::string emptyPostRaceActivityField;
    int copChance = pickRandomNumber(0, 75);
    std::vector<std::pair<std::string, int>> chosenTriggers;
    std::vector<std::string> triggerDetails;
    bool hasBossRaceTrigger = false;
    std::string prefix;

    for ( int loop = 0; loop < numberOfRaces + numberOfBossRaces; loop++ )
    {
        //Writes a comment that says which race node was chosen and as which race in the text file
        file << std::string("# ").append(raceBin).append(", race ")
        .append(std::to_string(loop + 1)).append("\n");

        //Pick race at random and retrieve its integer from raceMap
        pickedRace = pickRandomNumber(0, races.size() - 1); // NOLINT(cppcoreguidelines-narrowing-conversions)
        raceName = races[pickedRace].first;
        raceInteger = raceMap.find(raceName);

        //Race type is used both for naming triggers and knowing which fields to randomize later
        raceType = checkRaceType(raceInteger->second);

        //Set prefix for modifying triggers
        if ( raceName.substr(0, 2) == "16" ) { prefix = raceName; }
        else { prefix = raceName.substr(0, 11); }

        //Remove prologue fields, if originally prologue race
        if ( raceName.substr(0, 2) == "16" ) { convertPrologueRaceToRegularRace(file, raceName); }

        //Also check for a single Razor race that has a DDayRace field and remove it
        if ( raceName == "race_bin_01/1_5_2_speedtraprace" )
        { file << std::string("delete_field gameplay race_bin_01/1_5_2_speedtraprace DDayRace\n"); }
	
		//Check for a single Big Lou race that has a misspelled Name field and correct it
        if ( raceName == "race_bin_11/11_2_1_sprint" )
        { file << std::string("update_field gameplay race_bin_11/11_2_1_sprint Name race_bin_11/11_2_1_sprint\n"); }

        //Updates vault for race if originally from different blacklist member
        if ( raceBin != raceName.substr(0, 11) )
        {
            file << std::string("change_vault gameplay ").append(raceName).append(" ").append(raceBin)
            .append("\n");
        }

        //If regular race or first boss race, update or create race triggers. Skip this block if Track Expansion race
        if ( raceInteger->second < 175 )
        {
            //Get trigger data first
            triggerDetails = getNewTriggerDetails(raceInteger->second);

            if ( loop < numberOfRaces + 1 )
            {
                //If it is a vanilla race with a trigger, create prefix and add to vector
                //with race integer for engage node updates
                if ( hasTrigger(raceInteger->second) )
                {
                    chosenTriggers.emplace_back(prefix + triggerDetails[0], raceInteger->second );
                    if ( loop == numberOfRaces ) { hasBossRaceTrigger = true; }
                }

                //If it is a vanilla race without a trigger, create one and do the same as above
                else
                {
                    createRaceTrigger(file, raceName, raceType, raceBin, triggerDetails, prefix);
                    chosenTriggers.emplace_back(prefix + triggerDetails[0], raceInteger->second );
                    if ( loop == numberOfRaces ) { hasBossRaceTrigger = true; }
                }
            }

            //If a boss race past the first, delete the trigger
            else
            {
                //Track expansion races do not have triggers
                if ( raceInteger->second > 174 ) {}

                else if ( hasTrigger(raceInteger->second) )
                {
                    file << std::string("delete_node gameplay ").append(prefix)
                    .append(triggerDetails[0]).append("\n");
                }
            }
        }

        //If originally a boss race, convert to regular race
        if ( races[pickedRace].second && loop < numberOfRaces )
        {
            convertBossRaceToRegularRace(file, raceName, raceCashValue, raceInteger->second, opponentList,
                                         raceType, raceBin, startingOpponentList);
        }

        //Boss races are handled here
        else if ( loop >= numberOfRaces && loop < numberOfBossRaces + numberOfRaces )
        {
            emptyPostRaceActivityField = createBossRace(file, raceName, bossRaceCashValue, numberOfRaces,
                                            numberOfBossRaces, loop, raceInteger->second, raceType,
                                            bossOpponentString, raceBin, emptyPostRaceActivityField);
        }

        //If a race was a regular race and is selected to be a regular race, update some fields
        else
        {
            updateRegularRace(file, raceName, raceType, raceCashValue,
                            raceBin, raceInteger->second, opponentList, startingOpponentList);
        }

        //Randomize race intro after conversion
        if ( raceType != "tollbooth" && raceName != "16_1_1_circuit" )
        {
            if ( loop >= numberOfRaces ) { randomizeRaceIntro(file, raceName, raceInteger->second, true); }
            else { randomizeRaceIntro(file, raceName, raceInteger->second, false); }
        }

        //Replace a few necessary fields with the current race
        if ( loop < numberOfRaces )
        {
            file << std::string("update_field gameplay ").append(raceBin).append(" WorldRaces[")
            .append(std::to_string(loop)).append("] ").append(raceName).append("\n");
        }

        else
        {
            file << std::string("update_field gameplay ").append(raceBin).append(" BossRaces[")
            .append(std::to_string(bossRaceCounter)).append("] ").append(raceName).append("\n");
            bossRaceCounter++;
        }

        file << std::string("update_field gameplay ").append(raceBin).append(" Children[")
        .append(bossIndexes[loop]).append("] ").append(raceName).append("\n");

        if ( checkForField(raceInteger->second, 10) )
        { file << std::string("delete_field gameplay ").append(raceName).append(" IntroMovie").append("\n"); }

        randomizeGeneralFields(file, raceName, distanceMap, numberOfRaces, raceType,
                               raceInteger->second, loop, copChance);

        //A single newline character for any other function that writes to the file
        file << std::string("\n");

        //Remove processed race from pool
        races.erase(races.begin() + pickedRace);
    }

    //Use vector with triggers and race integers to update race bin engage node
    updateRaceTriggers(file, hasBossRaceTrigger, raceBin, chosenTriggers);

    //Return modified races vector for future function calls
    return races;
}

void updatePursuitRequirements(std::ofstream& file, int totalMilestones, int minimumBounty, const std::string& raceBin)
{
    std::vector<int> possibleMinimumMilestones;
    float numberOfMilestonesFloat;
    numberOfMilestonesFloat = (float)totalMilestones;

    file << std::string("# changing number of milestones and bounty for ").append(raceBin).append("\n");

    file << std::string("update_field gameplay ").append(raceBin).append(" RequiredBounty ")
    .append(std::to_string(minimumBounty)).append("\n");

    for ( int loop = 0; loop < totalMilestones; loop++ )
    {
        if ( ((float)loop + 1) / numberOfMilestonesFloat >= 0.40 &&
        ((float)loop + 1) / numberOfMilestonesFloat <= 0.80 )
        { possibleMinimumMilestones.emplace_back(loop + 1); }
    }

    file << std::string("update_field gameplay ").append(raceBin).append(" RequiredChallenges ")
    .append(std::to_string(possibleMinimumMilestones[pickRandomNumber(0,
    (int)possibleMinimumMilestones.size() - 1)])).append("\n\n");
}

std::vector<std::pair<std::string, bool>> randomizePrologue(int numberOfRaces, int raceCashValue,
                 std::vector<std::pair<std::string, bool>> races, const std::unordered_map<std::string, int>& raceMap,
                 const std::array<std::string, 41>& opponentList, const std::array<std::string, 73>& carList,
                 const std::unordered_map<int, float>& distanceMap, const std::array<std::string, 35>& aiCarList)
{
    //Main function for prologue
    races = randomizePrologueRaces(numberOfRaces, races, raceCashValue, raceMap,
                                   opponentList, carList, distanceMap, aiCarList);

    return races;
}

std::vector<std::pair<std::string, bool>> randomizeChapter(int regularRaces, int originalNumberOfRaces,
             int bossRaces, int originalNumberOfBossRaces, std::vector<std::pair<std::string, bool>> races,
             const std::unordered_map<std::string, int>& raceMap, const std::array<std::string, 41>& opponentList,
             const std::unordered_map<int, float>& distanceMap, const std::string& raceBin, int raceCashValue,
             int bossRaceCashValue, const std::string& bossOpponentString,
             const std::array<std::string, 6>& startingOpponentList, int heatLevel, int minimumBounty, int bountyGap,
             std::vector<std::string> bossIndexes)
{
    std::ofstream file;
    std::ofstream& fileReference = file;
    int chapterMilestones;
    int chapterSpeedtraps;

    file.open("Random.nfsms", std::ios_base::app);

    bossIndexes = updateNumberOfChapterRaces(fileReference, regularRaces,
           originalNumberOfRaces, bossRaces,
           originalNumberOfBossRaces, raceBin, bossIndexes);

    //Main function that modifies all races for the blacklist member
    races = randomizeRaces(fileReference, regularRaces, bossRaces,
           bossRaceCashValue, raceBin, bossOpponentString, races, raceCashValue, raceMap, opponentList,
           bossIndexes, distanceMap, startingOpponentList);

    //Milestones are randomized next
    randomizeMilestones(fileReference, heatLevel, bountyGap, raceBin);
    chapterMilestones = 4;

    //Speedtraps are next
    chapterSpeedtraps = randomizeSpeedtraps(fileReference, bountyGap, raceBin);

    //Change pursuit requirements based on last two function calls
    updatePursuitRequirements(fileReference, chapterSpeedtraps + chapterMilestones,
           minimumBounty, raceBin);

    //All done
    file.close();
    return races;
}

int main()
{
    //Race names, map and long name vectors are added from function at start
    std::vector<std::pair<std::string, bool>> races = populateRaces();
    std::unordered_map<std::string, int> raceMap = createRaceMap();
    std::unordered_map<int, float> distanceMap = createDistanceMap();
    std::array<std::string, 73> carList = getListOfPlayerCars();
    std::array<std::string, 35> aiCarList = getListOfAICars();
    std::array<std::string, 41> opponentList = getListOfRegularOpponents();
    std::array<std::string, 18> scriptedOpponentList = getListOfScriptedOpponents();
    std::array<std::string, 6> startingOpponentList = getListOfStartingOpponents();
    std::array<std::string, 46> cars = getListOfCars();
    std::array<std::string, 69> engineSounds = getListOfEngineSounds();
	
	//Integers for pre-randomization numbers of races
    int originalNumberOfSonnyRaces = 6;
    int originalNumberOfTazRaces = 6;
    int originalNumberOfVicRaces = 7;
    int originalNumberOfIzzyRaces = 7;
    int originalNumberOfBigLouRaces = 8;
    int originalNumberOfBaronRaces = 8;
    int originalNumberOfEarlRaces = 8;
    int originalNumberOfJewelsRaces = 7;
    int originalNumberOfKazeRaces = 10;
    int originalNumberOfMingRaces = 11;
    int originalNumberOfWebsterRaces = 10;
    int originalNumberOfJVRaces = 11;
    int originalNumberOfRonnieRaces = 12;
    int originalNumberOfBullRaces = 12;
    int originalNumberOfRazorRaces = 11;
	
	//Boss index vectors, each index is a spot taken by a race in the children field of that boss node
    std::vector<std::string> sonnyIndexes = { "1", "2", "3", "4", "7", "13", "14", "15" };
    std::vector<std::string> tazIndexes = { "1", "2", "3", "4", "5", "6", "7", "9" };
    std::vector<std::string> vicIndexes = { "1", "2", "3", "4", "5", "6", "7", "11", "12" };
    std::vector<std::string> izzyIndexes = { "1", "2", "3", "4", "5", "6", "10", "11", "12" };
    std::vector<std::string> bigLouIndexes = { "1", "2", "3", "4", "5", "6", "9", "10", "11", "15" };
    std::vector<std::string> baronIndexes = { "1", "2", "3", "4", "5", "9", "10", "11", "12", "16" };
    std::vector<std::string> earlIndexes = { "1", "2", "3", "4", "5", "7", "8", "9", "15", "16" };
    std::vector<std::string> jewelsIndexes = { "1", "3", "6", "7", "8", "9", "13", "14", "15" };
    std::vector<std::string> kazeIndexes = { "1", "2", "3", "4", "5", "6", "7", "9", "12", "13", "14", "18" };
    std::vector<std::string> mingIndexes = { "1", "2", "3", "4", "5", "6", "7", "8", "12", "13", "14", "15", "16" };
    
    std::vector<std::string> websterIndexes = 
            { "2", "3", "4", "5", "6", "7", "10", "11", "12", "13", "14", "15", "16" };
    
    std::vector<std::string> jvIndexes = 
            { "1", "2", "3", "4", "5", "6", "10", "11", "12", "13", "14", "15", "16", "20" };
    
    std::vector<std::string> ronnieIndexes = 
            { "1", "3", "4", "5", "8", "9", "10", "11", "12", "13", "14", "15", "19", "20", "21" };
    
    std::vector<std::string> bullIndexes = 
            { "2", "3", "4", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "21" };
    
    std::vector<std::string> razorIndexes = 
            { "2", "3", "4", "5", "6", "7", "10", "11", "12", "13", "14", "15", "16", "20", "21", "23" };

    //Variables for deciding number of races
    int numberOfRacesForBoss = returnNumberOfRaces(0, originalNumberOfSonnyRaces);
    int currentRaceVariation = numberOfRacesForBoss - originalNumberOfSonnyRaces;
    std::pair<int, int> bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 2);

    int numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    //Variables for pursuits
    int currentMinimumBounty = pickRandomNumber(20000, 40000);
    int previousMinimumBounty = currentMinimumBounty;
    int bountyGap = currentMinimumBounty;

    //Randomize all regular opponents from this function
    randomizeOpponents(opponentList, scriptedOpponentList, startingOpponentList, aiCarList);

    //Have all boroughs unlocked from start
    randomizeFreeRoam();

    //Change a few fields to make Black Edition races usable in career
    convertCollectorsEditionRaces();

    //Copy a single speedtrap node to use for race speedtraps
    createSpeedtrapTemplate();

    //Delete all milestones, keep only one of each type as a template
    resetMilestones();

    //Delete all but one of each marker type to use as templates
    resetPerformanceMarkers();
    resetVisualMarkers();
    resetRandomMarkers();

    //Prologue
    races = randomizePrologue(3, 10000, races, raceMap,
                              opponentList, carList, distanceMap, aiCarList);

    //Sonny
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfSonnyRaces,
        numberOfBossRacesForBoss, 2, races, raceMap, opponentList, distanceMap,
        "race_bin_15", 1500, 5000, "race_bin_15/tony_ho",
        startingOpponentList, 1, currentMinimumBounty, bountyGap, sonnyIndexes);

    numberOfRacesForBoss = returnNumberOfRaces(currentRaceVariation, originalNumberOfTazRaces);
    currentRaceVariation = currentRaceVariation + (numberOfRacesForBoss - originalNumberOfTazRaces);
    bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 2);

    numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 20000, previousMinimumBounty + 40000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;
    previousMinimumBounty = currentMinimumBounty;

    //Taz
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfTazRaces,
        numberOfBossRacesForBoss, 2, races, raceMap, opponentList, distanceMap,
        "race_bin_14", 2000, 6000, "race_bin_14/vince_kilic",
        startingOpponentList, 1, currentMinimumBounty, bountyGap, tazIndexes);

    numberOfRacesForBoss = returnNumberOfRaces(currentRaceVariation, originalNumberOfVicRaces);
    currentRaceVariation = currentRaceVariation + (numberOfRacesForBoss - originalNumberOfVicRaces);
    bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 2);

    numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 20000, previousMinimumBounty + 40000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;
    previousMinimumBounty = currentMinimumBounty;

    //Vic
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfVicRaces,
        numberOfBossRacesForBoss, 2, races, raceMap, opponentList, distanceMap,
        "race_bin_13", 2500, 6000, "race_bin_13/victor_vasquez",
        startingOpponentList, 1, currentMinimumBounty, bountyGap, vicIndexes);

    numberOfRacesForBoss = returnNumberOfRaces(currentRaceVariation, originalNumberOfIzzyRaces);
    currentRaceVariation = currentRaceVariation + (numberOfRacesForBoss - originalNumberOfIzzyRaces);
    bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 2);

    numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 100000, previousMinimumBounty + 200000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;
    previousMinimumBounty = currentMinimumBounty;

    //Izzy
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfIzzyRaces,
        numberOfBossRacesForBoss, 2, races, raceMap, opponentList, distanceMap,
        "race_bin_12", 3000, 8000, "race_bin_12/isabel_diaz",
        startingOpponentList, 2, currentMinimumBounty, bountyGap, izzyIndexes);

    numberOfRacesForBoss = returnNumberOfRaces(currentRaceVariation, originalNumberOfBigLouRaces);
    currentRaceVariation = currentRaceVariation + (numberOfRacesForBoss - originalNumberOfBigLouRaces);
    bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 2);

    numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 100000, previousMinimumBounty + 200000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;
    previousMinimumBounty = currentMinimumBounty;

    //Big Lou
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfBigLouRaces,
         numberOfBossRacesForBoss, 2, races, raceMap, opponentList, distanceMap,
         "race_bin_11", 4000, 10000, "race_bin_11/lou_park",
         startingOpponentList, 2, currentMinimumBounty, bountyGap, bigLouIndexes);

    numberOfRacesForBoss = returnNumberOfRaces(currentRaceVariation, originalNumberOfBaronRaces);
    currentRaceVariation = currentRaceVariation + (numberOfRacesForBoss - originalNumberOfBaronRaces);
    bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 2);

    numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 200000, previousMinimumBounty + 400000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;
    previousMinimumBounty = currentMinimumBounty;

    //Baron
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfBaronRaces,
         numberOfBossRacesForBoss, 2, races, raceMap, opponentList, distanceMap,
         "race_bin_10", 5000, 12000, "race_bin_10/karl_smit",
         startingOpponentList, 3, currentMinimumBounty, bountyGap, baronIndexes);

    numberOfRacesForBoss = returnNumberOfRaces(currentRaceVariation, originalNumberOfEarlRaces);
    currentRaceVariation = currentRaceVariation + (numberOfRacesForBoss - originalNumberOfEarlRaces);
    bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 2);

    numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 200000, previousMinimumBounty + 400000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;
    previousMinimumBounty = currentMinimumBounty;

    //Earl
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfEarlRaces,
          numberOfBossRacesForBoss, 2, races, raceMap, opponentList, distanceMap,
          "race_bin_09", 6000, 16000, "race_bin_09/pierre_dupont",
          startingOpponentList, 3, currentMinimumBounty, bountyGap, earlIndexes);

    numberOfRacesForBoss = returnNumberOfRaces(currentRaceVariation, originalNumberOfJewelsRaces);
    currentRaceVariation = currentRaceVariation + (numberOfRacesForBoss - originalNumberOfJewelsRaces);
    bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 2);

    numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 200000, previousMinimumBounty + 400000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;
    previousMinimumBounty = currentMinimumBounty;

    //Jewels
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfJewelsRaces,
          numberOfBossRacesForBoss, 2, races, raceMap, opponentList, distanceMap,
          "race_bin_08",8000, 20000, "race_bin_08/jade_barrett",
          startingOpponentList, 3, currentMinimumBounty, bountyGap, jewelsIndexes);

    numberOfRacesForBoss = returnNumberOfRaces(currentRaceVariation, originalNumberOfKazeRaces);
    currentRaceVariation = currentRaceVariation + (numberOfRacesForBoss - originalNumberOfKazeRaces);
    bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 2);

    numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 400000, previousMinimumBounty + 1000000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;
    previousMinimumBounty = currentMinimumBounty;

    //Kaze
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfKazeRaces,
          numberOfBossRacesForBoss, 2, races, raceMap, opponentList, distanceMap,
          "race_bin_07",10000, 24000, "race_bin_07/kira_nazakato",
          startingOpponentList, 3, currentMinimumBounty, bountyGap, kazeIndexes);

    numberOfRacesForBoss = returnNumberOfRaces(currentRaceVariation, originalNumberOfMingRaces);
    currentRaceVariation = currentRaceVariation + (numberOfRacesForBoss - originalNumberOfMingRaces);
    bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 2);

    numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 400000, previousMinimumBounty + 1000000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;
    previousMinimumBounty = currentMinimumBounty;

    //Ming
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfMingRaces,
          numberOfBossRacesForBoss, 2, races, raceMap, opponentList, distanceMap,
          "race_bin_06",12000, 28000, "race_bin_06/hector_domingo",
          startingOpponentList, 4, currentMinimumBounty, bountyGap, mingIndexes);

    numberOfRacesForBoss = returnNumberOfRaces(currentRaceVariation, originalNumberOfWebsterRaces);
    currentRaceVariation = currentRaceVariation + (numberOfRacesForBoss - originalNumberOfWebsterRaces);
    bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 3);

    numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 400000, previousMinimumBounty + 1000000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;
    previousMinimumBounty = currentMinimumBounty;

    //Webster
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfWebsterRaces,
          numberOfBossRacesForBoss, 3, races, raceMap, opponentList, distanceMap,
          "race_bin_05",14000, 48000, "race_bin_05/wes_allen",
          startingOpponentList, 4, currentMinimumBounty, bountyGap, websterIndexes);

    numberOfRacesForBoss = returnNumberOfRaces(currentRaceVariation, originalNumberOfJVRaces);
    currentRaceVariation = currentRaceVariation + (numberOfRacesForBoss - originalNumberOfJVRaces);
    bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 3);

    numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 400000, previousMinimumBounty + 1000000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;
    previousMinimumBounty = currentMinimumBounty;

    //JV
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfJVRaces,
          numberOfBossRacesForBoss, 3, races, raceMap, opponentList, distanceMap,
          "race_bin_04",16000, 54000, "race_bin_04/joe_vega",
          startingOpponentList, 4, currentMinimumBounty, bountyGap, jvIndexes);

    numberOfRacesForBoss = returnNumberOfRaces(currentRaceVariation, originalNumberOfRonnieRaces);
    currentRaceVariation = currentRaceVariation + (numberOfRacesForBoss - originalNumberOfRonnieRaces);
    bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 3);

    numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 1000000, previousMinimumBounty + 3000000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;
    previousMinimumBounty = currentMinimumBounty;

    //Ronnie
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfRonnieRaces,
          numberOfBossRacesForBoss, 3, races, raceMap, opponentList, distanceMap,
          "race_bin_03", 18000, 60000, "race_bin_03/ronnie_mccrea",
          startingOpponentList, 5, currentMinimumBounty, bountyGap, ronnieIndexes);

    numberOfRacesForBoss = returnNumberOfRaces(currentRaceVariation, originalNumberOfBullRaces);
    currentRaceVariation = currentRaceVariation + (numberOfRacesForBoss - originalNumberOfBullRaces);
    bossRacesResult = returnNumberOfBossRaces(currentRaceVariation, 3);

    numberOfBossRacesForBoss = bossRacesResult.second;
    currentRaceVariation = bossRacesResult.first;

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 1000000, previousMinimumBounty + 3000000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;
    previousMinimumBounty = currentMinimumBounty;

    //Bull
    races = randomizeChapter(numberOfRacesForBoss, originalNumberOfBullRaces,
          numberOfBossRacesForBoss, 3, races, raceMap, opponentList, distanceMap,
          "race_bin_02", 20000, 75000, "race_bin_02/toro_sato",
          startingOpponentList, 5, currentMinimumBounty, bountyGap, bullIndexes);

    numberOfRacesForBoss = originalNumberOfRazorRaces + (currentRaceVariation * -1);

    currentMinimumBounty = pickRandomNumber(previousMinimumBounty + 1000000, previousMinimumBounty + 3000000);
    bountyGap = currentMinimumBounty - previousMinimumBounty;

    //Razor
    randomizeChapter(numberOfRacesForBoss, originalNumberOfRazorRaces, 5,
          5, races, raceMap, opponentList, distanceMap, "race_bin_01", 25000,
          0, "race_bin_01/razor_callahan", startingOpponentList, 5,
          currentMinimumBounty, bountyGap, razorIndexes);

    //Cleanup
    deleteSpeedtrapTemplate();
    deleteMilestoneTemplates();
	deleteMarkerTemplates();

    //All done
    return 0;
}
