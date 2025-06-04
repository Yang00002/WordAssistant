#pragma once
#include <cassert>
#include <functional>
#include <random>
// 随机数种子
extern std::random_device RandomSeed;
//随机数引擎
//int number = std::uniform_int_distribution<int> intrd(min, max);
extern std::mt19937 RandomEngine;
/// <summary>
/// 产生一个int随机数
/// </summary>
/// <param name="min">随机数最小值</param>
/// <param name="max">随机数最大值</param>
/// <returns>生成的随机数</returns>
int RandomInt(const int& min, const int& max);
/// <summary>
/// 产生一个long long随机数
/// </summary>
/// <param name="min">随机数最小值</param>
/// <param name="max">随机数最大值</param>
/// <returns>生成的随机数</returns>
long long RandomLonglong(const long long& min, const long long& max);
/// <summary>
/// 产生一个float随机数
/// </summary>
/// <param name="min">随机数最小值</param>
/// <param name="max">随机数最大值</param>
/// <returns>生成的随机数</returns>
float RandomFloat(const float& min, const float& max);
/// <summary>
/// 产生一个double随机数
/// </summary>
/// <param name="min">随机数最小值</param>
/// <param name="max">随机数最大值</param>
/// <returns>生成的随机数</returns>
double RandomDouble(const double& min, const double& max);
/// <summary>
/// 产生一个bool随机数
/// </summary>
/// <returns>生成的随机数</returns>
bool RandomBool();
/// <summary>
/// 产生一个int随机数列
/// </summary>
/// <param name="begin">数列的递增迭代器，应确保数列有足够空间</param>
/// <param name="min">随机数最小值</param>
/// <param name="max">随机数最大值</param>
/// <param name="count">随机数数量</param>
/// <param name="isrepeat">是否允许重复随机数</param>
template<typename Iter>
void RandomIntList(Iter begin, const int& min, const int& max, int count, const bool& isrepeat) {
	assert(max >= min);
	assert(count > 0 && (isrepeat || static_cast<long long>(max) - min + 1 >= count));
	if (isrepeat)
	{
		std::uniform_int_distribution intrd(min, max);
		for (int i = 0; i < count; i++)
		{
			*begin = intrd(RandomEngine);
			++begin;
		}
	}
	else
	{
		std::list<int> result;
		for (int num = 0; num < count; num++, ++begin)
		{
			int number = std::uniform_int_distribution<int>(min, max - num)(RandomEngine);
			auto i = result.begin();
			for (; i != result.end(); ++i)
			{
				if (number >= *i)
					number++;
				else
					break;
			}
			result.insert(i, number);
			*begin = number;
		}
	}
}
/// <summary>
/// 产生一个int随机数列
/// </summary>
/// <param name="begin">数列的递增迭代器，应确保数列有足够空间</param>
/// <param name="insertplace">数列拆入位置，使用insertplace(*begin)代替*begin</param>
/// <param name="min">随机数最小值</param>
/// <param name="max">随机数最大值</param>
/// <param name="count">随机数数量</param>
/// <param name="isrepeat">是否允许重复随机数</param>
template<typename Iter>
void RandomIntList(Iter begin, std::function<int& (decltype(*begin)&)> insertplace, const int& min, const int& max, int count, const bool& isrepeat) {
	assert(max >= min);
	assert(count > 0 && (isrepeat || max - min + 1 >= count));
	if (isrepeat)
	{
		std::uniform_int_distribution intrd(min, max);
		for (int i = 0; i < count; i++)
		{
			insertplace(*begin) = intrd(RandomEngine);
			++begin;
		}
	}
	else
	{
		std::list<int> result;
		for (int num = 0; num < count; num++, ++begin)
		{
			int number = std::uniform_int_distribution<int>(min, max - num)(RandomEngine);
			auto i = result.begin();
			for (; i != result.end(); ++i)
			{
				if (number >= *i)
					number++;
				else
					break;
			}
			result.insert(i, number);
			insertplace(*begin) = number;
		}
	}
}

/// <summary>
/// 产生一个float随机数列
/// </summary>
/// <param name="begin">数列的递增迭代器，应确保数列有足够空间</param>
/// <param name="min">随机数最小值</param>
/// <param name="max">随机数最大值</param>
/// <param name="count">随机数数量</param>
template<typename Iter>
void RandomFloatList(Iter begin, const float& min, const float& max, int count)
{
	assert(max >= min);
	assert(count > 0);
	std::uniform_real_distribution intrd(min, max);
	for (int i = 0; i < count; i++)
	{
		*begin = intrd(RandomEngine);
		++begin;
	}
}
/// <summary>
/// 产生一个float随机数列
/// </summary>
/// <param name="begin">数列的递增迭代器，应确保数列有足够空间</param>
/// <param name="insertplace">数列拆入位置，使用insertplace(*begin)代替*begin</param>
/// <param name="min">随机数最小值</param>
/// <param name="max">随机数最大值</param>
/// <param name="count">随机数数量</param>
template<typename Iter>
void RandomFloatList(Iter begin, std::function<int& (decltype(*begin)&)> insertplace, const float& min, const float& max, int count)
{
	assert(max >= min);
	assert(count > 0);
	std::uniform_real_distribution intrd(min, max);
	for (int i = 0; i < count; i++)
	{
		insertplace(*begin) = intrd(RandomEngine);
		++begin;
	}
}
/// <summary>
/// 产生一个bool可重复随机数列
/// </summary>
/// <param name="begin">数列的递增迭代器，应确保数列有足够空间</param>
/// <param name="count">随机数数量</param>
template<typename Iter>
void RandomBoolList(Iter begin, int count) {
	assert(count > 0);
	std::uniform_int_distribution<int> intrd(0, 1);
	for (int i = 0; i < count; i++)
	{
		*begin = intrd(RandomEngine);
		++begin;
	}
}
/// <summary>
/// 产生一个bool可重复随机数列
/// </summary>
/// <param name="begin">数列的递增迭代器，应确保数列有足够空间</param>
/// <param name="insertplace">数列拆入位置，使用insertplace(*begin)代替*begin</param>
/// <param name="count">随机数数量</param>
template<typename Iter>
void RandomBoolList(Iter begin, std::function<int& (decltype(*begin)&)> insertplace, int count) {
	assert(count > 0);
	std::uniform_int_distribution<int> intrd(0, 1);
	for (int i = 0; i < count; i++)
	{
		insertplace(*begin) = intrd(RandomEngine);
		++begin;
	}
}
