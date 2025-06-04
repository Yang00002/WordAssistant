#include "RandHelp.hpp"
std::random_device RandomSeed;
std::mt19937 RandomEngine(RandomSeed());

int RandomInt(const int& min, const int& max) {
	assert(min <= max);
	std::uniform_int_distribution<int> intrd(min, max);
	return intrd(RandomEngine);
}
long long RandomLonglong(const long long& min, const long long& max)
{
	assert(min <= max);
	std::uniform_int_distribution<long long> intrd(min, max);
	return intrd(RandomEngine);
}
float RandomFloat(const float& min, const float& max) {
	assert(min <= max);
	std::uniform_real_distribution<float> realrd(min, max);
	return realrd(RandomEngine);
}

double RandomDouble(const double& min, const double& max) {
	assert(min <= max);
	std::uniform_real_distribution<double> realrd(min, max);
	return realrd(RandomEngine);
}
bool RandomBool() {
	std::uniform_int_distribution<int> intrd(0, 1);
	return intrd(RandomEngine);
}
