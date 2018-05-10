#include "stdafx.h"
#include "DiceRoller.h"
#include "utility.h"
#include <random>
#include <vector>
#include <chrono>

#define FUNCTION_PARSE_DICE(_Str_target,_Pos_Start,_Pos_D,_Pos_K,_Pos_L) \
int i_num_of_die = _Pos_D > 0 ? std::stoi(_Str_target.substr(_Pos_Start, _Pos_D - (_Pos_Start))) : 1;\
int i_face_of_die = std::stoi(_Str_target.substr(_Pos_D + 1, _Pos_K - (_Pos_D + 1)));\
int i_num_of_keep = std::stoi(_Str_target.substr(_Pos_L + 1))

#define CHECK_DICE_LIMITS() if (i_face_of_die > MAX_DICE_FACE || i_num_of_keep > MAX_DICE_FACE || i_num_of_die > MAX_DICE_NUM) break

#define CREATING_OUTPUT(_Stream,_DiceRoll,_Sign) \
_Stream << "(" << *(_DiceRoll.str_detail_result) << ")";\
this->i_sum_result = _DiceRoll.i_sum_result * _Sign;\
this->str_detail_result = new std::string(_Stream.str())

#define RANDOMIZE(_Max)\
std::random_device rd_generator;\
std::mt19937 mt_generator(ulong_prand_seed);\
if (DiceRoller::is_using_pseudo_random) mt_generator.discard(ulong_prand_stage);\
std::uniform_int_distribution<> dice(1, _Max)

#define RANDOM(_Target)\
if (DiceRoller::is_using_pseudo_random) { _Target = dice(mt_generator); ulong_prand_stage++;}\
else _Target = dice(rd_generator)

void DiceRoller::random_initialize()
{
	std::random_device rd;
	if (rd.entropy() > 0.0) {
		DiceRoller::is_using_pseudo_random = false;
	}
	else DiceRoller::is_using_pseudo_random = true;
}

bool DiceRoller::is_using_pseudo_random = false;
unsigned long DiceRoller::ulong_prand_seed = 0;
unsigned long DiceRoller::ulong_prand_stage = 0;

DiceRoller::DiceRoller() noexcept
{
}


DiceRoller::~DiceRoller()
{
	if (str_detail_result != nullptr)
		free(str_detail_result);
}

DiceRoller::DiceRoller(int val1_i_num_of_dice, int val2_num_of_face) {
	RANDOMIZE(val2_num_of_face);
	int i_result_sum = 0;
	std::ostringstream ostrs_dice_stream(std::ostringstream::ate);
	while (val1_i_num_of_dice > 0)
	{
		int i_step_result = 0;
		RANDOM(i_step_result);
		i_result_sum += i_step_result;
		ostrs_dice_stream << i_step_result;
		if ((--val1_i_num_of_dice) > 0) ostrs_dice_stream << " + ";
	}
	this->str_detail_result = new std::string(ostrs_dice_stream.str());
	this->i_sum_result = i_result_sum;
}

DiceRoller::DiceRoller(int num_of_dice, int num_of_face, int keep, bool is_keeping_high) {
	if (keep >= num_of_dice) {
		DiceRoller dice(num_of_dice, num_of_face);
		this->str_detail_result = new std::string(*dice.str_detail_result);
		this->i_sum_result = dice.i_sum_result;
	}
	else {
		RANDOMIZE(num_of_face);
		int i_result_sum = 0;
		std::ostringstream ostrs_dice_stream(std::ostringstream::ate);

		std::vector<int> resultList;
		std::vector<int> sortList;
		std::vector<int> pilotList;
		std::vector<int> flagList;

		for (int i_count = 0; i_count < num_of_dice; i_count++) {
			int i_temp_result = 0;
			RANDOM(i_temp_result);
			resultList.push_back(i_temp_result);
			sortList.push_back(i_temp_result);
			pilotList.push_back(i_count);
			flagList.push_back(0);
		}
		quickSort(sortList.data(), pilotList.data(), 0, sortList.size() - 1);

		if (is_keeping_high) {
			for (int i_iter = 0; i_iter < keep; i_iter++) {
				flagList[pilotList[i_iter]] = 1;
			}
		}
		else {
			for (int i_iter = 1; i_iter <= keep; i_iter++) {
				flagList[pilotList[pilotList.size() - i_iter]] = 1;
			}
		}

		for (int i_iter = 0; i_iter < num_of_dice; i_iter++) {
			if (flagList[i_iter] == 1) {
				i_result_sum += resultList[i_iter];
				ostrs_dice_stream << resultList[i_iter];
			}
			else {
				ostrs_dice_stream << "(" << resultList[i_iter] << ")";
			}
			if (i_iter + 1 < num_of_dice) ostrs_dice_stream << " + ";
		}
		this->str_detail_result = new std::string(ostrs_dice_stream.str());
		this->i_sum_result = i_result_sum;
	}
}

//inputs regex (\\+|\\-)?(\\d*d\\d+((k|kl)\\d+)?)|(\\d+)
DiceRoller::DiceRoller(std::string & str_single_dice)
{
	bool is_first_signed = (str_single_dice[0] == '+' || str_single_dice[0] == '-');
	std::ostringstream ostrs_dice_stream(std::ostringstream::ate);
	
	int i_dice_summary = 0;
	int i_single_dice_sign = 1;
	int i_start_pos = 0;

	if (str_single_dice[0] == '+' || str_single_dice[0] == '-') {
		ostrs_dice_stream << " " << str_single_dice[0] << " ";
		if (str_single_dice[0] == '-') i_single_dice_sign = -1;
		else i_single_dice_sign = 1;
		i_start_pos = 1;
	}

	do {
		int i_pos_of_d = str_single_dice.find('d');
		if (i_pos_of_d == std::string::npos) {
			int result = std::stoi(str_single_dice.substr(i_start_pos));
			this->i_sum_result = result * i_single_dice_sign;
			ostrs_dice_stream << result;
			this->str_detail_result = new std::string(ostrs_dice_stream.str());
		}
		else {
			int i_pos_of_k = str_single_dice.find('k');
			if (i_pos_of_k == std::string::npos) {
				int i_num_of_die = i_pos_of_d > 0 ? std::stoi(str_single_dice.substr(i_start_pos, i_pos_of_d)) : 1;
				int i_face_of_die = std::stoi(str_single_dice.substr(i_pos_of_d + 1));
				int i_num_of_keep = i_num_of_die;
				CHECK_DICE_LIMITS();
				DiceRoller dr_diceRoll(i_num_of_die, i_face_of_die);
				CREATING_OUTPUT(ostrs_dice_stream, dr_diceRoll, i_single_dice_sign);
			}
			else {
				int i_pos_of_l = str_single_dice.find('l');
				if (i_pos_of_l == std::string::npos) {
					i_pos_of_l = i_pos_of_k;
					FUNCTION_PARSE_DICE(str_single_dice, i_start_pos, i_pos_of_d, i_pos_of_k, i_pos_of_l);
					CHECK_DICE_LIMITS();
					DiceRoller dr_diceRoll(i_num_of_die, i_face_of_die, i_num_of_keep, false);
					CREATING_OUTPUT(ostrs_dice_stream, dr_diceRoll, i_single_dice_sign);
				}
				else {
					FUNCTION_PARSE_DICE(str_single_dice, i_start_pos, i_pos_of_d, i_pos_of_k, i_pos_of_l);
					CHECK_DICE_LIMITS();
					DiceRoller dr_diceRoll(i_num_of_die, i_face_of_die, i_num_of_keep, true);
					CREATING_OUTPUT(ostrs_dice_stream, dr_diceRoll, i_single_dice_sign);
				}
			}
		}
	} while (false);
}

