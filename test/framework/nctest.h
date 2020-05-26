#pragma once
#include <iostream>
#include <vector>

namespace nc::test
{

    class NC_TEST
    {
        public:
            NC_TEST()
                : testsRun(0), testsPassed(0)
            {
            }

            template<class T>
            void operator ()(const char * description, T func)
            {
                ++testsRun;
                std::cout << "Running Test: " << description << '\n';
                auto result = func();
                if(result)
                {
                    ++testsPassed;
                    std::cout << "  Pass\n";
                }
                else
                {
                    failedTestDescs.push_back(description);
                    std::cout << "  Fail\n";
                }
                
            }

            void DisplayResults()
            {
                std::cout << '\n' << testsPassed << '/' << testsRun << " tests passed\n\n"
                          << "The following tests failed:\n\n";
            
            for(auto& str : failedTestDescs)
            {
                std::cout << str << '\n';
            }

            std::cout << '\n';
            }

        private:
            unsigned int testsRun;
            unsigned int testsPassed;
            std::vector<std::string> failedTestDescs;
    };

} //end namespace nc::test