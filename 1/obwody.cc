#include <iostream>
#include <string>
#include <regex>
#include <unordered_set>
#include <unordered_map>

namespace
{
    // The element Id is represented as a pair of a character and integer.
    using Id = std::pair<char, int>;

    std::ostream& operator<<(std::ostream& os,  Id const& id)
    {
        os << id.first << id.second;
        return os;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream& os,  std::vector<T> const& v)
    {
        bool first_printed{ false };
        for(auto const& value : v)
        {
            if(first_printed)
                os << ", " << value;
            else
            {
                os << value;
                first_printed = true;
            }
        }
        return os;
    }
    // This regex will match any correct lines. The matching groups are
    // (starting from 1):
    // 1 - If this group is empty, this means the element is a transistor.
    //     Based on wheter or not this one is empty, you check the following
    //     groups to get the result.
    // If the element is transistor:
    //     5 - The Id of the element (type letter (D, R, C or E) + the number);
    //     6 - The type of the element;
    //     7, 8, 9 - Numbers of nodes for which the element is connected to.
    // Else:
    //     1 - The Id of the element (type letter (D, R, C or E) + the number);
    //     2 - The type of the element;
    //     3, 4 - Numbers of nodes for which the element is connected to.
    // This is a single text line, C++ preprocessor connects these strings.
    constexpr auto correct_line_regexp{
        "^\\s*"
        "(?:([DRCE](?:0|[1-9][0-9]{0,8}))\\s+"
        "((?:[A-Z]|[0-9])(?:[A-Za-z0-9]|,|-|\\/)*)\\s+"
        "(0|[1-9][0-9]{0,8})\\s+"
        "(0|[1-9][0-9]{0,8})|"
        "(?:(T(?:0|[1-9][0-9]{0,8}))\\s+"
        "((?:[A-Z]|[0-9])(?:[A-Za-z0-9]|,|-|\\/)*))\\s+"
        "(0|[1-9][0-9]{0,8})\\s+"
        "(0|[1-9][0-9]{0,8})\\s+"
        "(0|[1-9][0-9]{0,8}))"
        "\\s*$" };

    // Defines the prority for the given type when sorting the lists of
    // elements. The lower value, the higher proprity type has.
    const std::unordered_map<char, int> element_priority{
        { 'T', 1 }, { 'D', 2 }, { 'R', 3 }, { 'C', 4 }, { 'E', 5 } };

    // All fucntions are marked inline, because g++ on -O2 optimization 
    // will not try to inline fucntions unless it is explicitly marked so.
    
    // Compare two Ids. Ids are compared firstly based on type which is the
    // first element of the pair. If types are same, then we compare the integer
    // values.
    inline bool compare_ids(Id lhs, Id rhs)
    {
        if(lhs.first == rhs.first)
            return lhs.second <= rhs.second;
        return element_priority.at(lhs.first) <= element_priority.at(rhs.first);
    };

    inline bool parse_input_line(std::string const& line, 
        std::string &id, 
        std::string &name, 
        std::vector<int> &values)
    {
        std::smatch reg_match;
        std::regex reg_expression{ correct_line_regexp };

        // If line does not match the regex it is incorrect.
        if(!std::regex_search(line, reg_match, reg_expression))
            return false;

        if(reg_match[1] == "")
        {
            // In this case the object is a transistor, and it has 3 ends.
            id = reg_match[5];
            name = reg_match[6];

            values = std::vector<int> {
                std::stoi(reg_match[7]),
                std::stoi(reg_match[8]),
                std::stoi(reg_match[9])
            };
        }
        else
        {
            // Otherwise we must check the type by getting the first match
            // group.
            id = reg_match[1];
            name = reg_match[2];

            values = std::vector<int> {
                std::stoi(reg_match[3]),
                std::stoi(reg_match[4])
            };
        }

        // If the element is transistor it can be connected to the same node
        // twice, so we first make sure that all values are uniqe. Then if there
        // is only one value it means that the all values are the same, so we
        // return false.
        std::sort(values.begin(), values.end());
        auto last = std::unique(values.begin(), values.end());
        values.erase(last, values.end());

        if(values.size() == 1)
            return false;

        return true;
    }

    // Process the single input line. If line is empty it will do nothing,
    // otherwise it updates the data structures
    inline bool process_input_line(
        std::string const& line,
        std::unordered_set<std::string>& used_ids,
        std::unordered_map<std::string, std::vector<Id>>& name_to_ids,
        std::unordered_map<int, int>& node_to_connection_count)
    {
        if(line == "")
            return true;

        std::string id;
        std::string name;
        std::vector<int> values;
        if (!parse_input_line(line, id, name, values))
            return false;

        // If the second value of the pair returned by insert is false, this
        // means the value were already there and we should return false.
        bool insertion_result{ used_ids.insert(id).second };
        if(!insertion_result)
            return false;

        name_to_ids[name].emplace_back(id[0], std::stoi(id.c_str() + 1));

        // Increment connection count for each node.
        for(auto i : values)
            node_to_connection_count[i]++;

        return true;
    }

    // Compute the output.
    using result_type = std::vector<std::pair<std::string, std::vector<Id>>>;
    inline result_type compute_elements_list(
        std::unordered_map<std::string, std::vector<Id>> const& name_to_ids)
    {
        result_type result;
        result.reserve(name_to_ids.size());

        for(auto name_ids_pair : name_to_ids)
        {
            std::unordered_map<char, std::vector<Id>> types;
            for(auto& type : name_ids_pair.second)
                types[type.first].emplace_back(std::move(type));

            for(auto& type_vector : types)
            {
                result.emplace_back(name_ids_pair.first,
                                    std::move(type_vector.second));
            }
        }
        return result;
    }

    // Compute and print the output to the stdout.
    inline void compute_and_print_elements_list(
        std::unordered_map<std::string, std::vector<Id>> const& name_to_ids)
    {
        auto result = compute_elements_list(name_to_ids);

        for(auto& result_record : result)
        {
            std::sort(result_record.second.begin(),
                      result_record.second.end(),
                      compare_ids);
        }

        // Since every individual vector is sorted, and Ids does not repeat, we
        // just have to compare first elements.
        std::sort(result.begin(), result.end(),
                  [](auto x, auto y) {
                      return compare_ids(x.second[0], y.second[0]);
                  });

        for(auto key_value_pair : result)
            std::cout << key_value_pair.second << ": " 
                      << key_value_pair.first << '\n';
    }

    // Given a note to number of connected elements map, checks if any element
    // has less than two connected entities and if so, prints the warrning to
    // the stderr.
    inline void check_for_unconnected_nodes(
        std::unordered_map<int, int> const& node_to_connection_count)
    {
        std::vector<int> unconnected_nodes;
        for(auto const& i : node_to_connection_count)
            if(i.second < 2)
                unconnected_nodes.emplace_back(i.first);

        // If any unconnected node has been found print a warning to the stderr.
        if(unconnected_nodes.size() > 0)
        {
            std::sort(unconnected_nodes.begin(), unconnected_nodes.end());
            std::cerr << "Warning, unconnected node(s): "
                      << unconnected_nodes << '\n';
        }
    }
}

int main()
{
    std::unordered_set<std::string> used_ids;
    std::unordered_map<std::string, std::vector<Id>> name_to_ids;
    std::unordered_map<int, int> node_to_connection_count;

    // Node 0 always exists in the network.
    node_to_connection_count.insert({ 0, 0 });

    std::string line;
    for(std::size_t line_no{ 1 }; std::getline(std::cin, line); ++line_no)
    {
        if(!process_input_line(line, used_ids,
                               name_to_ids,
                               node_to_connection_count))
        {
            std::cerr << "Error in line " << line_no << ": " << line << '\n';
        }
    }

    compute_and_print_elements_list(name_to_ids);
    check_for_unconnected_nodes(node_to_connection_count);

    return 0;
}
