using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using System.Diagnostics;
using PlistCS;

namespace vcxProjGenerator
{
    class Program
    {
        static int ExeVersion=0x0106;   // be sure to just change number value

        static int Nb_Source_File_In_BigSourceFile = 40;

        static string[] FOLDERS_FOR_FILES = new string[] { 
            "src" 
        };

        static string[] FOLDERS = new string[] {
            "src"
        };

        static string BASE_URL = "";

        static string[] EXCLUDE_FILES = new string[]
        {
            "\\TestUIScene.h",
            "\\TestUIScene.cpp",
            "\\SystemUtilsBuildID.cpp"
        };

        static string[] EXCLUDE_FILES_WITH_TAG = new string[]
        {
            "ANDROID",
            "IOS",
            "MAC",
            "LINUX"
        };

        static string[] EXCLUDE_FOLDERS = new string[]
        {
            "android",
            "linux"
        };

        static string VcxSourceName = "baseVcxProj.txt";
        static string VcxFilterSourceName = "baseVcxProjFilter.txt";

        static string VcxDestiName = "HodEngine.vcxproj";
        static string VcxFilterDestiName = "HodEngine.vcxproj.filters";

        static String PathForBigCPP = "SourcesForBigCPP";
        static String PathForBigC = "SourcesForBigC";

        static string projWin32Path = "";

        //******************************************************************************
        //******************************************************************************
        //******************************************************************************
        //******************************************************************************

        class ArgsParamsDefinition
        {
            public enum ArgsParamsType
            {
                ReplaceString,
                GenBigCPP,
                GenBigC,
                GenBuildID,
                MaxValue
            }

            public enum ArgsParamsCat
            {
                GenType,
                GenBigFile,
                GenBuildID,
                MaxValue
            }

            public class ArgsParamsValue
            {
            }

            public class ArgsParamsReplaceString : ArgsParamsValue
            {
                public string m_OldString;
                public string m_NewString;

                public ArgsParamsReplaceString(string _old, string _new)
                {
                    m_OldString = _old;
                    m_NewString = _new;
                }
            }

            public class ArgsParamsGenBigFile : ArgsParamsValue
            {
                public bool m_doGenFlag;

                public ArgsParamsGenBigFile(bool _val)
                {
                    m_doGenFlag = _val;
                }
            }

            public class ArgsParamsGenBuildID : ArgsParamsValue
            {
                public bool m_doGenBuildID;

                public ArgsParamsGenBuildID(bool _val)
                {
                    m_doGenBuildID = _val;
                }
            }

            public string m_optionName;
            public ArgsParamsType   m_optionType;
            public ArgsParamsCat    m_optionCat;
            public ArgsParamsValue  m_optionValue;

            public ArgsParamsDefinition(string _name, ArgsParamsType _type, ArgsParamsCat _cat,ArgsParamsValue _value)
            {
                m_optionName = _name;
                m_optionType = _type;
                m_optionCat = _cat;
                m_optionValue = _value;
            }
        }

        static ArgsParamsDefinition[] ArgsParamsDefinitionList = new ArgsParamsDefinition[]
        {
            new ArgsParamsDefinition("-gen_for_testgame",ArgsParamsDefinition.ArgsParamsType.ReplaceString,ArgsParamsDefinition.ArgsParamsCat.GenType,
                                                        new ArgsParamsDefinition.ArgsParamsReplaceString(";__TEST_GAME__;",";TEST_GAME;")),
            new ArgsParamsDefinition("-gen_for_gamedesign",ArgsParamsDefinition.ArgsParamsType.ReplaceString,ArgsParamsDefinition.ArgsParamsCat.GenType,
                                                        new ArgsParamsDefinition.ArgsParamsReplaceString(";__GAME_DESIGN__;",";GAME_DESIGN;")),
            new ArgsParamsDefinition("-gen_big_cpp",ArgsParamsDefinition.ArgsParamsType.GenBigCPP,ArgsParamsDefinition.ArgsParamsCat.GenBigFile,
                                                        new ArgsParamsDefinition.ArgsParamsGenBigFile(true)),
            new ArgsParamsDefinition("-gen_big_c",ArgsParamsDefinition.ArgsParamsType.GenBigC,ArgsParamsDefinition.ArgsParamsCat.GenBigFile,
                                                        new ArgsParamsDefinition.ArgsParamsGenBigFile(true)),
            new ArgsParamsDefinition("-gen_build_id",ArgsParamsDefinition.ArgsParamsType.GenBuildID,ArgsParamsDefinition.ArgsParamsCat.GenBuildID,
                                                        new ArgsParamsDefinition.ArgsParamsGenBuildID(true))
        };

        //******************************************************************************
        //******************************************************************************
        //******************************************************************************
        //******************************************************************************

        static void Main(string[] args)
        {
            // check if version is the same than version inside source code
            string pathExe = Application.ExecutablePath;
            int pathExeIndexOfBin = pathExe.IndexOf("bin");
            string pathForSrc = pathExe.Substring(0, pathExeIndexOfBin);
            pathForSrc += "Program.cs";
            string source = File.ReadAllText(pathForSrc);
            int indexOfVersion = source.IndexOf("static int ExeVersion=");

            bool needToAskForRecompile = true;

            if(indexOfVersion<source.Length)
            {
                int indexOfStartVersion = source.IndexOf('0', indexOfVersion);
                int indexOfEndVersion = source.IndexOf(';', indexOfVersion);
                string versionString = source.Substring(indexOfStartVersion, indexOfEndVersion - indexOfStartVersion);
                int versionInSourceCode = Convert.ToInt32(versionString,16);
                if (versionInSourceCode == ExeVersion)
                    needToAskForRecompile = false;
            }

            if(needToAskForRecompile)
            {
                MessageBox.Show("Executable is not up to date.\nPlease regenerate .exe", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            int pathExeIndexOfProjWin32 = pathExe.IndexOf("vcxProjGenerator");
            projWin32Path = pathExe.Substring(0, pathExeIndexOfProjWin32);
            BASE_URL = projWin32Path + BASE_URL;

            string pathVcxSourceName = projWin32Path + VcxSourceName;
            if (!File.Exists(pathVcxSourceName))
            {
                MessageBox.Show("File " + pathVcxSourceName + " not found!!!", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            string pathVcxFilterSourceName = projWin32Path + VcxFilterSourceName;
            if (!File.Exists(pathVcxFilterSourceName))
            {
                MessageBox.Show("File " + pathVcxFilterSourceName + " not found!!!", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            string text = File.ReadAllText(pathVcxSourceName);
            string text_filter = File.ReadAllText(pathVcxFilterSourceName);

            // parse params
            bool[] locArgsFlags = new bool[ArgsParamsDefinitionList.Length];
            for (int locCountArgs = 0; locCountArgs < locArgsFlags.Length; locCountArgs++)
                locArgsFlags[locCountArgs]=false;

            bool[] locArgsCatFlags = new bool[ (int)ArgsParamsDefinition.ArgsParamsCat.MaxValue ];
            for (int locCountArgs = 0; locCountArgs < locArgsCatFlags.Length; locCountArgs++)
                locArgsCatFlags[locCountArgs] = false;

            for (int locCountArgs = 0; locCountArgs < args.Length; locCountArgs++)
            {
                int locArgFound = -1;
                int locCountTest = 0;
                while ((locCountTest < ArgsParamsDefinitionList.Length) && (locArgFound==-1) )
                {
                    if (ArgsParamsDefinitionList[locCountTest].m_optionName.Equals(args[locCountArgs]))
                    {
                        locArgsCatFlags[ (int)ArgsParamsDefinitionList[locCountTest].m_optionCat ] = true;
                        locArgsFlags[locCountTest] = true;
                        locArgFound = locCountTest;
                    }
                    locCountTest++;
                }
                if(locArgFound==-1)
                {
                    MessageBox.Show("Argument :  " + args[locCountArgs] + " not valid!!!", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }
            
            if (locArgsCatFlags[ (int)ArgsParamsDefinition.ArgsParamsCat.GenBuildID])
            {
                generateBuildID();
                return;
            }
            
            if( !locArgsCatFlags[ (int)ArgsParamsDefinition.ArgsParamsCat.GenType ] )
            {
                locArgsFlags[ doAskForGenerationType() ] = true;
            }

            bool _genBigCPP = false;
            bool _genBigC = false;

            // apply args param
            for (int locCountArgs = 0; locCountArgs < locArgsFlags.Length; locCountArgs++)
            {
                if (locArgsFlags[locCountArgs])
                {
                    if (ArgsParamsDefinitionList[locCountArgs].m_optionType == ArgsParamsDefinition.ArgsParamsType.ReplaceString)
                    {
                        ArgsParamsDefinition.ArgsParamsReplaceString locVal = (ArgsParamsDefinition.ArgsParamsReplaceString)ArgsParamsDefinitionList[locCountArgs].m_optionValue;
                        text = text.Replace(locVal.m_OldString, locVal.m_NewString);
                    }
                    else if (ArgsParamsDefinitionList[locCountArgs].m_optionType == ArgsParamsDefinition.ArgsParamsType.GenBigCPP)
                    {
                        _genBigCPP = ((ArgsParamsDefinition.ArgsParamsGenBigFile)ArgsParamsDefinitionList[locCountArgs].m_optionValue).m_doGenFlag;
                    }
                    else if (ArgsParamsDefinitionList[locCountArgs].m_optionType == ArgsParamsDefinition.ArgsParamsType.GenBigC)
                    {
                        _genBigC = ((ArgsParamsDefinition.ArgsParamsGenBigFile)ArgsParamsDefinitionList[locCountArgs].m_optionValue).m_doGenFlag;
                    }
                }
            }

            Directory.CreateDirectory(PathForBigCPP);
            Directory.CreateDirectory(PathForBigC);

            setFilterFolders(ref text_filter);

            if (_genBigCPP)
            {
                createBigSourceFile(ref text, ref text_filter, "*.cpp", "@BigCpp_Files", "@BigCpp_Compile", "Compile", PathForBigCPP, "BigCPP_", ".CPP");
            }
            else
            {
                text = text.Replace("@BigCpp_Files", "");
                text_filter = text_filter.Replace("@BigCpp_Compile", "");
            }

            if (_genBigCPP)
            {
                createBigSourceFile(ref text, ref text_filter, "*.c", "@BigC_Files", "@BigC_Compile", "Compile", PathForBigC, "BigC_", ".C");
            }
            else
            {
                text = text.Replace("@BigC_Files", "");
                text_filter = text_filter.Replace("@BigC_Compile", "");
            }

            setFiles(ref text, ref text_filter, "*.cpp", "@Cpp_Files", "@Cpp_Compile", "Compile", _genBigCPP);

            setFiles(ref text, ref text_filter, "*.c", "@C_Files", "@C_Compile", "Compile", _genBigC);

            setFiles(ref text, ref text_filter, "*.h", "@H_Files", "@H_Include", "Include", false);
            setFiles(ref text, ref text_filter, "*.hpp", "@Hpp_Files", "@Hpp_Include", "Include", false);

            setFolders(ref text, "@bfFolders" );

            string pathVcxDestiName = projWin32Path + VcxDestiName;
            TextWriter writer = new StreamWriter(pathVcxDestiName);
            writer.WriteLine(text);
            writer.Close();

            string pathVcxFilterDestiName = projWin32Path + VcxFilterDestiName;
            TextWriter writerFilters = new StreamWriter(pathVcxFilterDestiName);
            writerFilters.WriteLine(text_filter);
            writerFilters.Close();
        }

        static int doAskForGenerationType()
        {
            AskGenerateType locAskGenerateType = new AskGenerateType();

            for (int locCountArgs = 0; locCountArgs < ArgsParamsDefinitionList.Length; locCountArgs++)
            {
                if(ArgsParamsDefinitionList[locCountArgs].m_optionType == ArgsParamsDefinition.ArgsParamsType.ReplaceString)
                {
                    locAskGenerateType.AddGenerateChoice(ArgsParamsDefinitionList[locCountArgs].m_optionName);
                }
            }
            return locAskGenerateType.GetChoice();
        }

        static void getCppsFromFolder(string _folderPath, string _extension, ref string _toConstruct, ref string _toConstructForFilters, string _mode, bool _excludeFromBuild)
        {
            string[] files = Directory.GetFiles( _folderPath, _extension );
            int fileCount = files.Length;
            for ( int i = 0; i < fileCount; i++ )
            {
                FileInfo fileInfo = new FileInfo( files[i] );

                bool itMustContinue = false;
                foreach ( String fileToExclude in EXCLUDE_FILES )
                {
                    if ( fileInfo.FullName.ToUpper().EndsWith( fileToExclude.ToUpper() ))
                    {
                        itMustContinue = true;
                        break;
                    }
                }

                if (!itMustContinue)
                    itMustContinue = CheckNeedExcludeFile(fileInfo);

                if (itMustContinue)
                {
                    continue;
                }

                if (fileInfo.Name.Contains("android")
                    || fileInfo.Name.Contains("Android")
                    )
                {
                    continue;
                }

                string relativePathFolder = _folderPath.Remove(0, projWin32Path.Length);

                if (_excludeFromBuild)
                {
                    _toConstruct += string.Format("<Cl{2} Include=\"{0}\\{1}\">\n", relativePathFolder, fileInfo.Name, _mode);
                    _toConstruct += "<ExcludedFromBuild Condition=\"'$(Platform)'=='x64'\">true</ExcludedFromBuild>\n";
                    _toConstruct += string.Format("</Cl{0}>\n", _mode);
                }
                else
                {
                    string value = string.Format("<Cl{2} Include=\"{0}\\{1}\" />\n", relativePathFolder, fileInfo.Name, _mode);
                    _toConstruct += value;
                }
                
                string locFilter = _folderPath.Remove(0, BASE_URL.Length);
                _toConstructForFilters += string.Format("<Cl{2} Include=\"{0}\\{1}\"><Filter>{3}</Filter></Cl{2}>\n", relativePathFolder, fileInfo.Name, _mode, locFilter);
            }

            string[] folders = Directory.GetDirectories( _folderPath );
            int folderCount = folders.Length;
            for ( int i = 0; i < folderCount; i++ )
            {
                FileInfo fileInfo  = new FileInfo( folders[i] );

                if ( EXCLUDE_FOLDERS.Contains( fileInfo.Name ) )
                {
                    continue;
                }

                string newFolderPath = _folderPath + "\\" + fileInfo.Name;
                getCppsFromFolder(newFolderPath, _extension, ref _toConstruct,ref _toConstructForFilters, _mode,_excludeFromBuild);
            }
        }

        static void setFiles(ref string _text, ref string _text_filter, string _extension, string _token, string _tokenFilters, string _mode,bool _excludeFromBuild)
        {
            string result = "";
            string resultForFilters = "";
            for (int i = 0; i < FOLDERS_FOR_FILES.Length; i++)
            {
                getCppsFromFolder(BASE_URL + FOLDERS_FOR_FILES[i], _extension, ref result, ref resultForFilters, _mode , _excludeFromBuild);
            }

            _text_filter = _text_filter.Replace( _tokenFilters, resultForFilters);
            _text = _text.Replace( _token, result );
        }

        static void getFolders(string _folderPath, ref string _toConstruct)
        {
            _toConstruct += _folderPath + ";";
            string[] folders = Directory.GetDirectories(_folderPath);
            int folderCount = folders.Length;
            for (int i = 0; i < folderCount; i++)
            {
                FileInfo fileInfo = new FileInfo(folders[i]);
                
                if (EXCLUDE_FOLDERS.Contains(fileInfo.Name))
                {
                    continue;
                }

                string newFolderPath = _folderPath + "\\" + fileInfo.Name;
                getFolders(newFolderPath, ref _toConstruct);
            }
        }

        static void setFolders(ref string _text, string _token)
        {
            string result = "";
            for (int i = 0; i < FOLDERS.Length; i++)
            {
                getFolders(BASE_URL + FOLDERS[i], ref result);
            }

            _text = _text.Replace(_token, result);
        }

        //****************************************************************************************
        //****************************************************************************************
        //****************************************************************************************
        //****************************************************************************************
        static void setFilterFolders_ScanFolders(string _folderPath, ref List<string> _listOfFolders)
        {
            string locPathCheck = _folderPath.Remove(0,BASE_URL.Length);
            if (locPathCheck.IndexOf("\\") > 0)
            {
                string[] locPaths = locPathCheck.Split(Path.DirectorySeparatorChar);
                for(int locCC=0;locCC<locPaths.Length;locCC++)
                {
                    string locPathToAdd = BASE_URL+locPaths[0];
                    for(int locDD=1;locDD<=locCC;locDD++)
                    {
                        locPathToAdd += Path.DirectorySeparatorChar + locPaths[locDD];
                    }
                    if(_listOfFolders.IndexOf(locPathToAdd)==-1)
                        _listOfFolders.Add(locPathToAdd);
                }
            }
            else
            {
                if (_listOfFolders.IndexOf(_folderPath) == -1)
                    _listOfFolders.Add(_folderPath);
            }


            string[] folders = Directory.GetDirectories(_folderPath);
            int folderCount = folders.Length;
            for (int i = 0; i < folderCount; i++)
            {
                FileInfo fileInfo = new FileInfo(folders[i]);

                if (EXCLUDE_FOLDERS.Contains(fileInfo.Name))
                {
                    continue;
                }

                string newFolderPath = _folderPath + "\\" + fileInfo.Name;
                setFilterFolders_ScanFolders(newFolderPath, ref _listOfFolders);
            }
        }

        static void setFilterFolders(ref string _text_filter)
        {
            List<string> _locListOfFolders = new List<string>();
            for (int i = 0; i < FOLDERS_FOR_FILES.Length; i++)
            {
                setFilterFolders_ScanFolders(BASE_URL + FOLDERS_FOR_FILES[i], ref _locListOfFolders);
            }

            string locPortionFilter = "";
            for(int i=0; i< _locListOfFolders.Count;i++)
            {
                string locPath = _locListOfFolders[i];
                locPath = locPath.Remove(0, BASE_URL.Length);
                locPortionFilter += "    <Filter Include=\"" + locPath + "\"></Filter>\n";
            }
            locPortionFilter = locPortionFilter.Remove(locPortionFilter.Length - 1);
            _text_filter = _text_filter.Replace("@filter_zone_def", locPortionFilter);
        }

        //****************************************************************************************
        //****************************************************************************************
        //****************************************************************************************
        //****************************************************************************************
        static bool CheckNeedExcludeFile(FileInfo _fileInfo)
        {
            string _fileinfo_upper = _fileInfo.FullName.ToUpper();
            string _fileinfo_upper_withoutext = System.IO.Path.GetFileNameWithoutExtension(_fileinfo_upper);

            foreach (String fileTagToExclude in EXCLUDE_FILES_WITH_TAG)
            {
                if (_fileinfo_upper_withoutext.EndsWith(fileTagToExclude.ToUpper()))
                {
                    return true;
                }
            }
            return false;
        }

        static void BIGSRCFILE_getSourceFromFolder(string _folderPath, string _extension, ref List<string> _listOfFiles)
        {
            string[] files = Directory.GetFiles(_folderPath, _extension);
            int fileCount = files.Length;
            for (int i = 0; i < fileCount; i++)
            {
                FileInfo fileInfo = new FileInfo(files[i]);

                bool itMustContinue = false;
                foreach (String fileToExclude in EXCLUDE_FILES)
                {
                    if (fileInfo.FullName.ToUpper().EndsWith(fileToExclude.ToUpper()))
                    {
                        itMustContinue = true;
                        break;
                    }
                }

                if (!itMustContinue)
                    itMustContinue = CheckNeedExcludeFile(fileInfo);

                if (itMustContinue)
                {
                    continue;
                }

                if (fileInfo.Name.Contains("android")
                    || fileInfo.Name.Contains("Android")
                    )
                {
                    continue;
                }

                string value = string.Format("{0}\\{1}", _folderPath, fileInfo.Name);
                _listOfFiles.Add(value);
            }

            string[] folders = Directory.GetDirectories(_folderPath);
            int folderCount = folders.Length;
            for (int i = 0; i < folderCount; i++)
            {
                FileInfo fileInfo = new FileInfo(folders[i]);

                if (EXCLUDE_FOLDERS.Contains(fileInfo.Name))
                {
                    continue;
                }

                string newFolderPath = _folderPath + "\\" + fileInfo.Name;
                BIGSRCFILE_getSourceFromFolder(newFolderPath, _extension, ref _listOfFiles);
            }
        }

        static void createBigSourceFile(ref string _text, ref string _text_filter, string _extension, string _token, string _tokenFilter, string _mode, string _PathForBigCPP, string _bigsourcename, string _bigsourceext)
        {
            string locLinesForCompileBigCPP = "";

            string locLinesForBigCPPFilters = "";

            List<string> _locListOfFiles = new List<string>();
            for (int i = 0; i < FOLDERS_FOR_FILES.Length; i++)
            {
                BIGSRCFILE_getSourceFromFolder(BASE_URL + FOLDERS_FOR_FILES[i], _extension, ref _locListOfFiles);
            }

            // generate bigCPP
            int locCountBigCPP = 0;
            int locCountFiles = _locListOfFiles.Count;
            int locIndexFiles = 0;
            while(locCountFiles>0)
            {
                string locBigCPP_Filename = _bigsourcename + (locCountBigCPP + 1) + _bigsourceext;
                string locBigCPP_text = "//"+locBigCPP_Filename+"\n";

                int locNbFilesInBigCPP = locCountFiles > Nb_Source_File_In_BigSourceFile ? Nb_Source_File_In_BigSourceFile : locCountFiles;
                while(locNbFilesInBigCPP>0)
                {
                    string absolutePathToCpp = _locListOfFiles[locIndexFiles];
                    string relativePathToCpp = absolutePathToCpp.Remove(0, projWin32Path.Length);

                    locBigCPP_text += "#include \"..\\" + relativePathToCpp + "\"\n";
                    locNbFilesInBigCPP--;
                    locCountFiles--;
                    locIndexFiles++;
                }

                TextWriter writer = new StreamWriter( _PathForBigCPP+"\\"+locBigCPP_Filename);
                writer.WriteLine(locBigCPP_text);
                writer.Close();

                string value = string.Format("    <Cl{2} Include=\"{0}\\{1}\" />\n", _PathForBigCPP, locBigCPP_Filename, _mode);
                locLinesForCompileBigCPP += value;

                locLinesForBigCPPFilters += string.Format("    <Cl{2} Include=\"{0}\\{1}\"><Filter>win32</Filter></Cl{2}>\n", _PathForBigCPP, locBigCPP_Filename, _mode);

                locCountBigCPP++;
            }

            //locLinesForCompileBigCPP = locLinesForBigCPPFilters.Remove(locLinesForCompileBigCPP.Length - 1);
            _text = _text.Replace(_token, locLinesForCompileBigCPP);

            //locLinesForBigCPPFilters = locLinesForBigCPPFilters.Remove(locLinesForBigCPPFilters.Length - 1);
            _text_filter = _text_filter.Replace(_tokenFilter, locLinesForBigCPPFilters);
        }

        //***********************************************************************************************************************
        //***********************************************************************************************************************
        //***********************************************************************************************************************
        //***********************************************************************************************************************
        //***********************************************************************************************************************
        //***********************************************************************************************************************
        //***********************************************************************************************************************
        //***********************************************************************************************************************
        static string accoundID_textToSearch = "std::string SystemUtils::buildID";
        static string date_textToSearch = "std::string SystemUtils::buildDate";
        static string branch_textToSearch = "std::string SystemUtils::buildBranch";
        static string gameVersion_textToSearch = "std::string SystemUtils::gameVersion";

        static void generateBuildID_FindAndReplace(ref string filecontent, string whatToFind,string whatToPut)
        {
            int indexOfFind = filecontent.IndexOf(whatToFind);
            indexOfFind += whatToFind.Length;
            int indexOfInsert = filecontent.IndexOf("\"", indexOfFind) + 1;
            int indexOfEndInsert = filecontent.IndexOf("\"", indexOfInsert+1);

            filecontent = filecontent.Remove(indexOfInsert, indexOfEndInsert - indexOfInsert);
            filecontent = filecontent.Insert(indexOfInsert, whatToPut);
        }

        static void generateBuildID()
        {
            // get user name
            string accoundBuildID = Environment.UserName;

            // get date
            string dateBuildID = DateTime.Now.ToString("yyyy-MM-dd HH:mm");

            // get branch
            Process process = new Process();
            process.StartInfo.FileName = "cmd.exe";
            process.StartInfo.Arguments = "/C git rev-parse --abbrev-ref HEAD";
            process.StartInfo.RedirectStandardOutput = true;
            process.StartInfo.UseShellExecute = false;
            process.Start();
            process.WaitForExit();// Waits here for the process to exit.
            string branchBuildID = process.StandardOutput.ReadToEnd().Trim();

            string gameVersionBuildID = "0.0.0";
            string infoPlistFilePath = projWin32Path + "\\..\\proj.ios_mac\\mac\\Info.plist";
            Console.WriteLine("infoPlistFilePath: " + infoPlistFilePath);
            
            if (File.Exists(infoPlistFilePath) == false)
            {
                MessageBox.Show("File " + infoPlistFilePath + " not found!!!", "ERROR", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            Dictionary<string, object> dict = (Dictionary<string, object>)Plist.readPlist(infoPlistFilePath);
            object gameVersionBuildIDObject;
            dict.TryGetValue("CFBundleShortVersionString", out gameVersionBuildIDObject);
            gameVersionBuildID = (string)gameVersionBuildIDObject;
            Console.WriteLine("gameVersionBuildID: " + gameVersionBuildID);

            // find src to modify
            string pathForSrc = projWin32Path + "\\..\\Classes\\engine\\system\\SystemUtilsBuildID.cpp";

            string content = "";
            content += "// FILE GENERATED DO NOT EDIT\n";
            content += "#include \"SystemUtils.hpp\"\n";
            content += "\n";
            content += "namespace gumiengine\n";
            content += "{\n";
            content += "    std::string SystemUtils::buildID = \"CyrilDarkVador\";	// LINE AUTO GENERATED, EDIT THIS LINE AT YOUR OWN RISK\n";
            content += "    std::string SystemUtils::buildDate = \"YYYY-MM-DD HH:SS\";	// LINE AUTO GENERATED, EDIT THIS LINE AT YOUR OWN RISK\n";
            content += "    std::string SystemUtils::buildBranch = \"undefined branch name\";	// LINE AUTO GENERATED, EDIT THIS LINE AT YOUR OWN RISK\n";
            content += "    std::string SystemUtils::gameVersion = \"1.0.0\";	// LINE AUTO GENERATED, EDIT THIS LINE AT YOUR OWN RISK\n";
            content += "}\n";
            content += "\n";
            File.WriteAllText(pathForSrc, content);

            string source = File.ReadAllText(pathForSrc);

            // insert Build ID
            generateBuildID_FindAndReplace(ref source, accoundID_textToSearch, accoundBuildID);
            generateBuildID_FindAndReplace(ref source, date_textToSearch, dateBuildID);
            generateBuildID_FindAndReplace(ref source, branch_textToSearch, branchBuildID);
            generateBuildID_FindAndReplace(ref source, gameVersion_textToSearch, gameVersionBuildID);

            File.WriteAllText(pathForSrc, source);
        }
    }
}