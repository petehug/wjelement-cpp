// wjelement.cpp : Defines the entry point for the console application.
//

/*
  validate.c: a proof-of-concept json-schema validator
  thanks to xiaoping.x.liu@intel.com

  after installing libwjelement (and running ldconfig if needed)...
  gcc -o validate -lwjelement -lwjreader validate.c
*/

#include "HeapChecker.h"
// HeapChecker	HeapChecker::M_HeapChecker(1617L);
HeapChecker	HeapChecker::M_HeapChecker;

#include "OutboundHttpRequest.h"
#include <wjelement++.h>
#include "Exception.h"
#include <stdexcept>
#include <iostream>
#include <fstream>

#include <json/json.h>
#include <wjelement.h>
#include <wjreader.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FILE_PATH_SEPARATOR
# ifdef _WIN32
#		define FILE_PATH_SEPARATOR "\\"
# else
#		define FILE_PATH_SEPARATOR "/"
# endif
#endif

using namespace WJPP;
using namespace TWO::HTTP;


XplBool	loadCB(WJElement parent, char *path, void *data, const char *file, const int line)
{
	using namespace std;

	WJElement			prnt = parent;
	string				strSep = string("/");
	string				strJP;

	if (path)
		strJP = strSep + path;

	while (prnt)
	{
		if (prnt->parent)
		{
			if (!prnt->name)
				strJP = strSep + strJP;
			else
				strJP = strSep + prnt->name + strJP;
		}
		else
		{
			if (prnt->name)
				strJP = strSep + prnt->name + strJP;
		}

		prnt = prnt->parent;
	}

	strJP = string("#") + strJP;

	if (data)
		cout << strJP << " - " << data << " (" << line << ")" << endl;
	else
		cout << strJP << " (" << line << ")" << endl;

	return 1;
}




Node loadDocument(JSONHttpRequest& httpRequest, const string& strURI)
{
	WJReader			reader = NULL;
	Node				doc;
	string				data;

	data = httpRequest.sendRequest(strURI);

	reader = WJROpenMemDocument((void*) data.c_str(), NULL, 0);

	if (!reader)
		return doc;

	doc = WJEOpenDocument(reader, NULL, NULL, NULL);
	WJRCloseDocument(reader);

	if (!doc)
		return Node();

	return doc;
}






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
	WJReader									reader = NULL;
	Node											testData;
	JSONHttpRequest						httpRequest;
	ostringstream							ostrm;
	string										data;
	NodeVect									vectTests;
	bool											fail = false;
	int												testFilter[] = { 0, 27, 0, 0 };  // [0]: if 0 ignore, the others represent the test you want
	ostringstream							currtest;

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

			testData = Node::parseJson(json);

			if (testData)
			{
				int j = 0;

				// shows test descriptions
				for (Node::iterator itr1 = testData.begin(); itr1 != testData.end(); itr1++)
				{
					j++;

					if (testFilter[0] && testFilter[2] && j != testFilter[2]) continue;

					Node	test = *itr1;
					Node	schema = test["schema"];

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
						Node subtests = test["tests"];
						int k = 0;

						for (Node::iterator itr2 = subtests.begin(); itr2 != subtests.end(); itr2++)
						{
							k++;

							if (testFilter[0] && testFilter[3] && k != testFilter[3]) continue;

							Node		subtest = *itr2;
							Node		testdata = subtest["data"];

							testdata.detach();

							bool shoudSucceed = subtest["valid"].getBool();

							cout << endl;
							cout << "        -----------------------------------------------------------" << endl;
							cout << "        " << (i + 1) << "." << j << "." << k << ". " << subtest["description"].getString() << endl;
							cout << "        should " << (shoudSucceed ? "" : "not ") << "validate" << endl;
							cout << "        data: ";

							if (testdata.isContainer())
							{
								testdata.dump(cout, 8);
							}
							else
							{
								testdata.dump(cout, 8);
								cout << endl;
							}


							if (schema.validateInstance(testdata) == shoudSucceed)
							{
								cout << "        SUCCESS\n";
							}
							else
							{
								fail = true;
								cout << "        FAILURE\n";
								testdata.dump(cout, 8);
							}

							testdata.discard();
						}
					}

					schema.discard();
				}
			}

			testData.discard();
			i++;
		}
	}
	catch (...)
	{
		D3::GenericExceptionHandler("main() caught an exception.");
	}

	return 0;
}
