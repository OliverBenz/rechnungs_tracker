#pragma once

#include <vector>

#include "bill.hpp"
#include "shop.hpp"
#include "category.hpp"

namespace bill {

bool addBillToFile(const bill& newBill);

std::vector<bill> getBills(const std::string& date);
std::vector<shop> getShops();

std::vector<category> getCategories();
std::vector<subcategory> getSubCategories(int categoryId);

}
