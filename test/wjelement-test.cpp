// wjelement-test.cpp : Simply tests wjelement-cpp against JSON schema draft-04.
//

#ifdef _WIN32
#include "HeapChecker.h"
// HeapChecker	HeapChecker::M_HeapChecker(1617L);
HeapChecker	HeapChecker::M_HeapChecker;
#endif

#include <wjelement++.h>

#include <iostream>
#include <fstream>
#include <stdexcept>

#include "Exception.h"

#ifndef FILE_PATH_SEPARATOR
# ifdef _WIN32
#		define FILE_PATH_SEPARATOR "\\"
# else
#		define FILE_PATH_SEPARATOR "/"
# endif
#endif

using namespace WJPP;
using namespace std;

int main(int argc, char **argv)
{
	string	names[] = {	
		"additionalItems",
		"additionalProperties",
		"allOf",
		"anyOf",
		"definitions",
		"dependencies",
		"enum",
		"items",
		"maximum",
		"maxItems",
		"maxLength",
		"maxProperties",
		"minimum",
		"minItems",
		"minLength",
		"minProperties",
		"multipleOf",
		"not",
		"oneOf",
		"pattern",
		"patternProperties",
		"properties",
		"ref",
		"refRemote",
		"required",
		"type",
		"uniqueItems"
	};

	Cache&										cache = WJPP::Cache::GetCache();
	string										fileName, testPath = string(".." FILE_PATH_SEPARATOR ".." FILE_PATH_SEPARATOR "draft4-tests" FILE_PATH_SEPARATOR);
	ostringstream							ostrm;
	NodeVect									vectTests;
	bool											fail = false;
	unsigned int							testFilter[] = { 0, 0, 0 };  // if you want all from test 3 do { 3, 0, 0 }, all from test 23.4 do { 23, 4, 0 } or just test 12.5.2 do { 12, 5, 2 } or leave all 0 to disable all filtering
	ostringstream							currtest;
	Node											tests, test, schema, subtests, subtest, data, errors;

	try
	{
		for (unsigned int i = 0; i < sizeof(names)/sizeof(string); i++)
		{
			if (testFilter[0] && (i+1) != testFilter[0]) continue;

			fileName = testPath + names[i] + ".json";

			cout << endl << endl << endl;
			cout << "=========================================================================" << endl;
			cout << (i + 1) << ". " << fileName << endl;

			std::ifstream		testFile(fileName.c_str());
			std::string			json((istreambuf_iterator<char>(testFile)), istreambuf_iterator<char>());

			if (json.empty())
				throw runtime_error(fileName + " could not be read or is empty");
 
			tests = Node::parseJson(json);

			if (tests)
			{
				unsigned int j = 0;

				// shows test descriptions
				for (Node::iterator itr1 = tests.begin(); itr1 != tests.end(); itr1++)
				{
					j++;

					if (testFilter[1] && j != testFilter[1]) continue;

					test = *itr1;
					schema = test["schema"];

					if (schema)
					{
						// unbind the schema so it can be cached
						schema.detach();

						cout << endl << endl;
						cout << "    _____________________________________________________________________" << endl;
						cout << "    " << (i + 1) << "." << j << ". " << test["description"].getString() << endl;
						cout << "    Schema:" << endl;

						schema.dump(cout, 4);

						// validate and register the schema
						if (!cache.loadSchema(schema, errors))
						{
							schema.dump(cout, 4);
							cout << "    FAILURE SCHEMA\n";
							errors.dump(cout, 4);

							errors.discard();
							schema.discard();
							continue;
						}
						else
						{
							cout << "    SUCCESS SCHEMA\n";
						}

						// deal with individual tests
						subtests = test["tests"];
						unsigned int k = 0;

						for (Node::iterator itr2 = subtests.begin(); itr2 != subtests.end(); itr2++)
						{
							k++;

							if (testFilter[2] && k != testFilter[2]) continue;

							subtest = *itr2;
							data = subtest["data"];

							data.detach();

							bool shouldSucceed = subtest["valid"].getBool();

							cout << endl;
							cout << "        -----------------------------------------------------------" << endl;
							cout << "        " << (i + 1) << "." << j << "." << k << ". " << subtest["description"].getString() << endl;
							cout << "        should " << (shouldSucceed ? "" : "not ") << "validate" << endl;
							cout << "        data: ";

							data.dump(cout, 8);

							if (!data.isContainer())
								cout << endl;

							if (schema.validateInstance(data, errors) == shouldSucceed)
							{
								if (errors)
								{
									cout << "        VALIDATION ERRORS\n";
									errors.dump(cout, 8);
								}

								cout << "        SUCCESS\n";
							}
							else
							{
								fail = true;

								if (errors)
								{
									cout << "        VALIDATION ERRORS\n";
									errors.dump(cout, 8);
								}

								cout << "        FAILURE\n";
								data.dump(cout, 8);
							}

							errors.discard();
							data.discard();
						}
					}

					schema.discard();
				}
			}

			tests.discard();
			i++;
		}
	}
	catch (...)
	{
		handleGenericException();
		data.discard();
		schema.discard();
		tests.discard();
	}

	return 0;
}
