#ifndef PRINT_PARSER_HPP
#define PRINT_PARSER_HPP

#include <forward_list>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <utility>

using namespace std::literals;

using color_t = const std::pair<std::string_view, std::string_view>;

const auto pattern = " \t\n\r"sv;

// trim from end of string (right)
constexpr std::string_view rtrim(std::string_view s,
                                 std::string_view t = pattern) {
  return s.substr(0, s.find_last_not_of(t) + 1);
}

// trim from beginning of string (left)
constexpr std::string_view ltrim(std::string_view s,
                                 std::string_view t = pattern) {
  const auto pos = s.find_first_not_of(t);
  return s.substr(pos == std::string::npos ? s.size() : pos);
}

// trim from both ends of string (left & right)
constexpr std::string_view trim(std::string_view s,
                                std::string_view t = pattern) {
  return ltrim(rtrim(s, t), t);
}

// std::string_view -> std::string_view -> bool
std::function<bool(std::string_view)> is_token(std::string_view str);

// TOKENS
constexpr auto t_background = "background:";
constexpr auto t_foreground = "color:";
constexpr auto t_font_weight = "font-weight:";
constexpr auto t_styled = "%c";
constexpr auto t_semicolon = ";";
constexpr auto t_space = " ";

constexpr auto t_font_bold = "bold";
constexpr auto t_font_normal = "normal";

const std::map<std::string_view, std::string_view> colors{
    {"red", "255;0;0"},      {"green", "0;128;0"},     {"brown", "162;45;45"},
    {"pink", "255;192;203"}, {"orange", "255;165;0"},  {"yellow", "255;255;0"},
    {"blue", "0;0;255"},     {"white", "255;255;255"}, {"purple", "128;0;128"},
    {"gray", "128;128;128"}, {"cyan", "0;255;255"},    {"black", "0;0;0"},
};

const std::map<std::string_view, std::string_view> font_weights{{"normal", "0"},
                                                                {"bold", "1"}};

std::string styled_text(std::string_view str);
std::pair<std::string_view, std::size_t>
get_token_color(std::string_view str) noexcept;
std::pair<std::string_view, std::size_t>
get_token_font_weight(std::string_view str) noexcept;

std::string console_parser(const std::string &str) {
  const std::string_view text{trim(str)};

  if (is_token(text)(t_styled)) {
    return styled_text(text.substr(strlen(t_styled)));
  }
  return str;
}

std::string styled_text(std::string_view str) {
  std::string background;
  std::string foreground;
  std::string font_weight;
  std::string_view text;

  for (std::size_t next = 0; next < str.size();) {
    auto substr = str.substr(next);
    if (auto is = is_token(substr);
        is(t_background) || is(t_foreground) || is(t_font_bold)) {
      text = str.substr(0, next);
      break;
    }
    next += 1;
  }

  auto substr = str.substr(text.size());
  std::cout << text << ", " << substr;

  for (std::size_t next = text.size(); next < substr.size();) {

    if (auto is = is_token(substr); is(t_background)) {
      next = strlen(t_background);
      auto r = get_token_color(substr.substr(next));
      background = std::get<0>(r);
      next += std::get<1>(r);

    } else if (is(t_foreground)) {
      next = strlen(t_foreground);
      auto r = get_token_color(substr.substr(next));
      foreground = std::get<0>(r);
      next += std::get<1>(r);

    } else if (is(t_font_weight)) {
      next = strlen(t_font_weight);
      auto r = get_token_font_weight(substr.substr(next));
      font_weight = std::get<0>(r);
      next += std::get<1>(r);

    } else {
      next += 1;
    }
  }

  std::string ftext = "\033[";
  ftext += font_weight;

  if (!background.empty()) {
    ftext += ";48;2;" + background;
  }

  if (!foreground.empty()) {
    ftext += ";38;2;" + foreground;
  }

  return ftext.append("m").append(text);
}

std::pair<std::string_view, std::size_t>
get_token_color(std::string_view str) noexcept {
  std::size_t next{0};
  for (; next != str.size();) {
    auto is = is_token(str.substr(next, next + 1));
    if (is(t_semicolon)) {
      break;
    }
    next += 1;
  }

  if (auto c = trim(str.substr(next)); colors.count(c) > 0) {
    return {colors.at(c), next};
  }

  return {"", 0};
}

std::pair<std::string_view, std::size_t>
get_token_font_weight(std::string_view str) noexcept {
  std::size_t next{0};
  for (; next != str.size();) {
    auto is = is_token(str.substr(next, next + 1));
    if (is(t_semicolon)) {
      break;
    }
    next += 1;
  }

  if (std::string_view c = trim(str.substr(next)); font_weights.count(c) > 0) {
    return {font_weights.at(c), next};
  }

  return {"", 0};
}

std::function<bool(std::string_view)> is_token(std::string_view str) {
  return [=](std::string_view token) -> bool {
    return token == str.substr(0, token.size());
  };
}

#endif
