// wjelement-test.cpp : Simply tests wjelement-cpp against JSON schema draft-04.
//

#ifdef _WIN32
#include "HeapChecker.h"
// HeapChecker	HeapChecker::M_HeapChecker(1617L);
HeapChecker	HeapChecker::M_HeapChecker;
#endif

#include <iostream>
#include <fstream>
#include <stdexcept>

#include <wjelement++.h>

#include "OutboundHttpRequest.h"
#include "Exception.h"

#ifndef FILE_PATH_SEPARATOR
# ifdef _WIN32
#		define FILE_PATH_SEPARATOR "\\"
# else
#		define FILE_PATH_SEPARATOR "/"
# endif
#endif

using namespace WJPP;
using namespace TWO::HTTP;
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

	Cache											cache;
	string										fileName, testPath = string(".." FILE_PATH_SEPARATOR ".." FILE_PATH_SEPARATOR "draft4-tests" FILE_PATH_SEPARATOR);
	JSONHttpRequest						httpRequest;
	ostringstream							ostrm;
	NodeVect									vectTests;
	bool											fail = false;
	int												testFilter[] = { 0, 27, 0, 0 };  // [0]: if 0 ignore, the others represent the test you want
	ostringstream							currtest;
	Node											tests, test, schema, subtests, subtest, data;

	try
	{
		for (unsigned int i = 0; i < sizeof(names)/sizeof(string); i++)
		{
			if (testFilter[0] && testFilter[1] && (i+1) != testFilter[1]) continue;

			fileName = testPath + names[i] + ".json";

			cout << endl << endl << endl;
			cout << "=========================================================================" << endl;
			cout << (i + 1) << ". " << fileName << endl;

			std::ifstream		testFile(fileName.c_str());
			std::string			json((istreambuf_iterator<char>(testFile)), istreambuf_iterator<char>());

			tests = Node::parseJson(json);

			if (tests)
			{
				int j = 0;

				// shows test descriptions
				for (Node::iterator itr1 = tests.begin(); itr1 != tests.end(); itr1++)
				{
					j++;

					if (testFilter[0] && testFilter[2] && j != testFilter[2]) continue;

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
						if (!cache.loadSchema(schema))
						{
							schema.dump(cout, 4);
							cout << "    FAILURE SCHEMA\n";

							schema.discard();
							continue;
						}
						else
						{
							cout << "    SUCCESS SCHEMA\n";
						}

						// deal with individual tests
						subtests = test["tests"];
						int k = 0;

						for (Node::iterator itr2 = subtests.begin(); itr2 != subtests.end(); itr2++)
						{
							k++;

							if (testFilter[0] && testFilter[3] && k != testFilter[3]) continue;

							subtest = *itr2;
							data = subtest["data"];

							data.detach();

							bool shoudSucceed = subtest["valid"].getBool();

							cout << endl;
							cout << "        -----------------------------------------------------------" << endl;
							cout << "        " << (i + 1) << "." << j << "." << k << ". " << subtest["description"].getString() << endl;
							cout << "        should " << (shoudSucceed ? "" : "not ") << "validate" << endl;
							cout << "        data: ";

							data.dump(cout, 8);

							if (!data.isContainer())
								cout << endl;


							if (schema.validateInstance(data) == shoudSucceed)
							{
								cout << "        SUCCESS\n";
							}
							else
							{
								fail = true;
								cout << "        FAILURE\n";
								data.dump(cout, 8);
							}

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
		D3::GenericExceptionHandler("main() caught an exception.");
		data.discard();
		schema.discard();
		tests.discard();
	}

	return 0;
}
