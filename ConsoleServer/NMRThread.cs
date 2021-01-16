using System;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;


namespace nmrServerThread
{
	/// <summary>
	/// Summary description for NMRThread.
	/// </summary>
	/// 
	public class NMRThread
	{
		// VarianNucTable and VarianSolventPPM will be filled on first use
		private static Dictionary<string, double> VarianNucTable = new Dictionary<string, double>();
		private static Dictionary<string, double> VarianSolventPPM = new Dictionary<string, double>();

		private Socket socket;
        private string ID;

		public NMRThread(Socket newsocket, int threadnumber)
		{
			socket = newsocket;
            ID = threadnumber.ToString();
            Thread.CurrentThread.CurrentCulture = new CultureInfo("en-US");
		}

		private string ReadLine(NetworkStream stream)
		{
			StringBuilder sb = new StringBuilder(64);
			while (true) 
			{
				int ich = 0;
				try
				{
					ich = stream.ReadByte();
				}
				catch
				{
					return "";
				}
				if (ich < 0)
					return "";
				Char ch = Convert.ToChar(ich);
				if (ch == '\n') break;
				sb.Append(ch);
			}
			return sb.ToString();
		}

		private void WriteLine(NetworkStream stream, string line)
		{
			byte[] bytes = Encoding.ASCII.GetBytes(line + "\n");
			stream.Write(bytes, 0, bytes.Length);
		}

        private string[] GetSubdirectories(string name)
        {
            string[] result;
            try
            {
                result = Directory.GetDirectories(name);
            }
            catch (UnauthorizedAccessException)
            {
                result = new string[0];
            }
            return result;
        }

		public void Serve()
		{
			NetworkStream nmrStream = new NetworkStream(socket);
			while (nmrStream.CanRead) 
			{
				string command = ReadLine(nmrStream);
				if (0 == command.Length) 
				{
					nmrStream.Close();
					socket.Shutdown(SocketShutdown.Send);
					socket.Close();
					return;
				}
                Console.WriteLine(ID + ": Command=" + command);
				switch (command[0]) 
				{
					case 'u':
						WriteLine(nmrStream, "ok");
						break;
					case '/':
						string dir = command.Substring(1);
						if (! dir.EndsWith(@"\"))
							dir = dir + @"\";
						Directory.SetCurrentDirectory(dir);
						if (Directory.Exists("nmr")) 
						{
							if (1 == GetSubdirectories(".").Length)
							{
								if (dir.Equals(@"..\"))
									Directory.SetCurrentDirectory(dir);
								else
									Directory.SetCurrentDirectory("nmr");
							}
						}
						dir = Directory.GetCurrentDirectory();
						if (! dir.EndsWith(@"\"))
							dir = dir + @"\";
						WriteLine(nmrStream, dir);
						break;
					case 'f':
						GetFile(nmrStream, command.Substring(1));
						break;
					case '1':
						GetSpectrum(nmrStream, false, command.Substring(1));
						break;
					case '2':
						GetSpectrum(nmrStream, true, command.Substring(1));
						break;
					case 'l':
						ListDirectory(nmrStream, false, true);
						break;
					case 'L':
						ListDirectory(nmrStream, true, true);
						break;
					case 'M':
						ListDirectory(nmrStream, true, false);
						break;
					case 't':
						WriteLine(nmrStream, "Omega");
						break;
					case 'x':
					case 'X':
						nmrStream.Close();
						break;
					case '?':
						if (File.Exists(command.Substring(1)))
							WriteLine(nmrStream, "YES");
						else
							WriteLine(nmrStream, "NO");
						break;
				}
			}
			socket.Shutdown(SocketShutdown.Send);
			socket.Close();
		}

		private string[] NumberDirs(string name)
		{
			string[] alldirs = GetSubdirectories(name);
			int count = alldirs.Length;
			ArrayList numbers = new ArrayList(count);
			if (count > 0)
			{
				count = 0;
				foreach (string fdir in alldirs)
				{
					string dir = Path.GetFileName(fdir);
					bool isnumber = true;
					for (int i=0; i<dir.Length; i++) 
					{
						if (! Char.IsDigit(dir, i)) 
						{
							isnumber = false;
							break;
						}
					}
					if (isnumber)
						numbers.Add(Convert.ToInt32(dir));
				}
			}
			count = numbers.Count;
			string[] result = new string[count];
			if (count > 0)
			{
				numbers.Sort();
				for (int i=0; i<count; i++)
					result[i] = numbers[i].ToString();
			}
			return result;
		}

		private string ReadFile(string name)
		{
			if (! File.Exists(name))
				return "";
			FileInfo fi = new FileInfo(name);
			if (fi.Length >= 1048576L)
				return "";
			int count = Convert.ToInt32(fi.Length);
			FileStream fs = new FileStream(name, FileMode.Open, FileAccess.Read);
			if (fs == null || ! fs.CanRead)
				return "";
			byte[] buffer = new byte[count];
			int offset = 0;
			while (count > 0) 
			{
				int thisread = fs.Read(buffer, offset, count);
				offset += thisread;
				count -= thisread;
			}
			fs.Close();
			return Encoding.ASCII.GetString(buffer);
		}

		private string[] StrSplit(string str, string split)
		{
			int i = str.IndexOf(split);
			if (i >= 0) 
			{
				string[] result = new string[2];
				result[0] = str.Substring(0, i).Trim();
				i += split.Length;
				result[1] = str.Substring(i).Trim();
				return result;
			} 
			else 
			{
				string[] result = new string[1];
				result[0] = str;
				return result;
			}
		}

        private void MapProcparFile(Dictionary<string, string[]> param, string file)
        {
            string[] line = file.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
            int i = 0;
            char[] nulchar = new char[] { };
            while (i < line.Length)
            {
                string[] word = line[i++].Split(nulchar, StringSplitOptions.RemoveEmptyEntries);
                if (11 == word.Length && 64 == Convert.ToInt32(word[10]))
                {
                    string key = word[0];
                    int basictype = Convert.ToInt32(word[2]);
                    word = line[i++].Split(nulchar, 2, StringSplitOptions.RemoveEmptyEntries);
                    switch (basictype)
                    {
                        case 1:
                            word = word[1].Split(nulchar, StringSplitOptions.RemoveEmptyEntries);
                            param.Add(key, word);
                            break;
                        case 2:
                            int size = Convert.ToInt32(word[0]);
                            string[] value = new string[size];
                             for (int j = 0; j < size; j++) {
                                if (0 == j)
                                    value[j] = word[1].Trim();
                                else
                                    value[j] = line[i++].Trim();
                                while (!value[j].EndsWith("\""))
                                    value[j] += "\n" + line[i++].Trim();
                                value[j] = value[j].Trim(new char[] { '"' });
                            }
							param.Add(key, value);
                            break;
                    }
                }
            }
        }

        private void MapJCAMPFile(Dictionary<string, string> param, string file)
		{
			string[] next = StrSplit(file, "\n##");
			while (2==next.Length)
			{
				next = StrSplit(next[1], "\n##");
				if (next.Length==2 && 0==next[0].Substring(0,1).CompareTo("$"))
				{
					string[] keyvalue = StrSplit(next[0], "=");
					if (2 == keyvalue.Length) 
					{
						if (keyvalue[1].IndexOf("(0..") == 0) 
						{
							string[] arry = keyvalue[1].Split(new Char[] {')'});
							arry = arry[1].Split(new Char[] {' ', '\n', '\r'}, StringSplitOptions.RemoveEmptyEntries);
							for (int i=0; i<arry.Length; i++)
								param[keyvalue[0].Substring(1)+i.ToString()] = arry[i].Trim();
						} 
						else 
						{
							param[keyvalue[0].Substring(1).Trim()] = keyvalue[1].Trim(new Char[] {'"', '<', '>'});
						}
					}
				}
			}
		}

		private void MapInfoFile(Dictionary<string,string> fh, string file)
		{
			string[] lines = file.Split(new char[] {'\n'});
			for (int i=0; i+1<lines.Length; i+=2)
			{
				string key = lines[i].Trim();
				if (key.Length > 0)
				{
					string val = "\n" + lines[i+1].Trim();
					switch (key)
					{
						case "CUSTOMER":
							fh["user"] = val;
							break;
						case "SAMPLE":
							fh["sample"] = val;
							break;
						case "COMMENT":
							fh["comment"] = val;
							break;
						case "WORK_ORDER":
							fh["workorder"] = val;
							break;
						case "PROJECT":
							fh["project"] = val;
							break;
						case "SUBMISSION_DATE":
							fh["submitted"] = val;
							break;
                        case "DATA_CLASS":
                            fh["dataclass"] = val;
                            break;
					}
				}
			}					
		}

		private void MapPropFile(Dictionary<string,string> fh, string file)
		{
			foreach (string line in file.Split(new char[] {'\n'}))
			{
				if (line.Length>28 && 0==line.Substring(0, 2).CompareTo("I_"))
				{

					int i = line.IndexOf("=NM=");
					if (i > 0)
					{
						string str = line.Substring(i+4);
						i = str.IndexOf(", HEIGHT=");
						if (i > 0)
							str = str.Substring(0, i);
						string[] keyval = str.Split(new char[] {','}, 2);
						if (keyval.Length == 2)
						{
							string key = keyval[0].Trim().ToLower();
							if (key.Length > 0) 
							{
								string val = keyval[1].Trim();
								if (val.Length>4)
									val = val.Substring(4);
								fh[key] = "\n" + val;
							}
						}
					}
				}
			}
		}

		private void ListDirectory(NetworkStream stream, bool onlyspectra, bool onlyoned)
		{
			if (null != Directory.GetParent("."))
				WriteLine(stream, @"..\");
			foreach (string namedir in GetSubdirectories(".")) 
			{
				string name = Path.GetFileName(namedir);
				bool fidfound = false;
				if (name.EndsWith(".fid"))
				{
					if (File.Exists(name + @"\procpar"))
					{
						fidfound = true;
						string file = ReadFile(name + @"\procpar");
						Dictionary<string, string[]> procpar = new Dictionary<string, string[]>();
						MapProcparFile(procpar, file);
						string[] value;
						int ni = 0;
						if (procpar.TryGetValue("ni", out value))
							ni = Convert.ToInt32(value[0]);
						if (File.Exists(name + @"\fid"))
						{
							if (!onlyspectra && 0==ni)
								WriteLine(stream, name);
						}
						if (fidfound)
						{
						if (File.Exists(name + @"\datdir\phasefile") && ((onlyoned && 0 == ni) || (!onlyoned && ni > 0)))
							{
								if (File.Exists(name + @"\datdir\data") || ni > 0)
								{
									int i = name.LastIndexOf('.');
									string proname = name.Substring(0, i + 1) + "pro";
									WriteLine(stream, proname);
								}
							}
						}
					}
				}
				if (!fidfound) // there is no Varian/Agilent file
				{
					string[] edirs = NumberDirs(name);
					foreach (string expno in edirs)
					{
						string path = name + @"\" + expno;
						fidfound = File.Exists(path + @"\ser")
								&& File.Exists(path + @"\acqus")
								&& File.Exists(path + @"\acqu2s");
						if (onlyoned)
						{
							if (fidfound)
							{
								if (!File.Exists(path + @"\difflist"))
								{
									fidfound = false;
									string acqus = ReadFile(path + @"\acqus");
									Dictionary<string, string> param = new Dictionary<string, string>();
									MapJCAMPFile(param, acqus);
									foreach (string parname in new string[] { "VDLIST", "VALIST", "VPLIST", "VCLIST", "VTLIST" })
									{
										if (param.ContainsKey(parname))
										{
											if (File.Exists(path + @"\" + param[parname]) || File.Exists(path + @"\" + parname.ToLower()))
											{
												fidfound = true;
												break;
											}
										}
									}
									param.Clear();
								}
							}
							else
								fidfound = File.Exists(path + @"\acqus") && File.Exists(path + @"\fid");
						}
						if (fidfound)
						{
							if (!onlyspectra)
								WriteLine(stream, path);
							path = path + @"\pdata";
							if (Directory.Exists(path))
							{
								foreach (string procno in NumberDirs(path))
								{
									string pname = path + @"\" + procno;
									bool specfound = File.Exists(pname + @"\2rr") && File.Exists(pname + @"\procs");
									if (!onlyspectra)
										specfound = specfound && File.Exists(pname + @"\2ii");
									if (onlyoned)
									{
										if (!specfound)
										{
											specfound = File.Exists(pname + @"\1r")
												&& File.Exists(pname + @"\1i")
												&& File.Exists(pname + @"\procs");
										}
									}
									if (specfound)
										WriteLine(stream, pname);
								}
							}
						}
					}
					if (0 == edirs.Length)
					{
						if (!((File.GetAttributes(name) & FileAttributes.Hidden) == FileAttributes.Hidden))
							WriteLine(stream, name + @"\");
					}
				}
			}
			WriteLine(stream, ".");
		}

		private void GetFile(NetworkStream stream, string reqfilename)
		{
            if (reqfilename.EndsWith(".fid") || reqfilename.EndsWith(".pro"))
                GetVarianFile(stream, reqfilename);
            else
                GetBrukerFile(stream, reqfilename);
        }

        private void GetVarianFile(NetworkStream stream, string reqfilename)
        {
			SaneHD hd = new SaneHD(896);
			string sequel = "sequel\n";
			string dosy = "DOSY\n";
			string nl = "\n";
			int dimension = 1;
			int ftflag = 0;
			string fbase = reqfilename;
			if (reqfilename.EndsWith(".pro"))
			{
				ftflag = 1;
				fbase = reqfilename.Substring(0, reqfilename.Length - 3) + "fid";
			}
			hd.InsertChar('2', 603);
			hd.Insert(dimension, 296);
			hd.Insert(ftflag, 252);
			string file = ReadFile(fbase + @"\procpar");
			Dictionary<string, string[]> procpar = new Dictionary<string,string[]>();
			MapProcparFile(procpar, file);
			Dictionary<string, string> fh = new Dictionary<string, string>();
			string array = "";
			string[] arrayvalue = new string[0];
			string[] value;
			if (procpar.TryGetValue("array", out value))
			{
				array = value[0];
				if (array.Length > 0)
					procpar.TryGetValue(array, out arrayvalue);
			}
			if (procpar.TryGetValue("at", out value))
				hd.Insert(Convert.ToDouble(value[0]), 112);
			if (procpar.TryGetValue("ct", out value))
				hd.Insert(Convert.ToInt32(value[0]), 356);
			if (procpar.TryGetValue("d1", out value))
			{
				double d1 = Convert.ToDouble(value[0]);
				if (d1 > 0.0)
					fh.Add("D1", sequel + value[0] + "s");
			}
			if (procpar.TryGetValue("d2", out value))
			{
				double d2 = Convert.ToDouble(value[0]);
				if (d2 > 0.0)
					fh.Add("D2", sequel + value[0] + "s");
			}
			if (procpar.TryGetValue("DAC_to_G", out value))
				fh.Add("DAC_to_G", dosy + value[0]);
			if (procpar.TryGetValue("date", out value))
				fh.Add("date", nl + value[0]);
			double f2freq = 0.0;
			if (procpar.TryGetValue("dfrq", out value))
			{
				if (value[0].Length > 0)
				{
					f2freq = Convert.ToDouble(value[0]);
					hd.Insert(f2freq, 528);
				}
			}
			string dm;
			if (procpar.TryGetValue("dm", out value))
			{
				dm = value[0];
				fh.Add("DecouplerMode", nl + dm);
			}
			if (procpar.TryGetValue("dmf", out value))
				hd.Insert(Convert.ToSingle(value[0]) * 0.25f, 552);
			if (procpar.TryGetValue("dmm", out value))
			{
				switch (value[0][0])
				{
				case 'c':
					hd.Insert(1, 568);
					break;
				case 'f':
					hd.Insert(10, 572);
					break;
				case 'n':
					hd.Insert(11, 572);
					break;
				case 'r':
					hd.Insert(0, 572);
					break;
				case 'g':
					hd.Insert(9, 572);
					break;
				case 'm':
					hd.Insert(1, 572);
					break;
				case 'w':
					hd.Insert(3, 572);
					break;
				default:
					hd.Insert(999, 572);
					break;
				}
			}
			string dnstr = "";
			if (procpar.TryGetValue("dn", out value))
				dnstr = value[0];
			if (dnstr.Length < 2)
				dnstr = "off";
			hd.Insert(dnstr, 604);
			double dof = 0.0;
			if (procpar.TryGetValue("dof", out value))
				dof = Convert.ToDouble(value[0]);
			if (procpar.TryGetValue("dosygamma", out value))
				fh.Add("dosygamma", dosy + value[0]);
			if (procpar.TryGetValue("dosytimecubed", out value))
				fh.Add("timecubed", dosy + value[0]);
			if (procpar.TryGetValue("dpwr", out value))
				hd.Insert(Convert.ToInt32(value[0]), 564);
			if (procpar.TryGetValue("gain", out value))
				hd.Insert(Convert.ToSingle(value[0]), 752);
			bool nosampleinfo = true;
			if (procpar.TryGetValue("gesample", out value))
			{
				nosampleinfo = false;
				fh.Add("sample", nl + value[0]);
				if (procpar.TryGetValue("gecomment", out value))
					fh.Add("comment", nl + value[0]);
				if (procpar.TryGetValue("gecustomer", out value))
					fh.Add("user", nl + value[0]);
				if (procpar.TryGetValue("geanalysis", out value))
					fh.Add("workorder", nl + value[0]);
				if (procpar.TryGetValue("geproject", out value))
					fh.Add("project", nl + value[0]);
				if (procpar.TryGetValue("gesubmitted", out value))
					fh.Add("submitted", nl + value[0]);
				if (procpar.TryGetValue("gedataclass", out value))
					fh.Add("dataclass", nl + value[0]);
			}
			if (procpar.TryGetValue("homo", out value))
			{
				if ('y' == value[0][0])
					hd.InsertChar('1', 603);
			}
			double lref = 0.0;
			if (procpar.TryGetValue("lockfreq_", out value)) 
			{
				lref = Convert.ToDouble(value[0]);
				hd.Insert(lref, 776);
			}
			if (procpar.TryGetValue("lockgain_", out value))
				hd.Insert(Convert.ToInt32(value[0]), 812);
			if (procpar.TryGetValue("lockpower_", out value))
				hd.Insert(Convert.ToInt32(value[0]), 816);
			if (procpar.TryGetValue("np", out value))
			{
				dimension = Convert.ToInt32(value[0]) / 2;
				hd.Insert(dimension, 284);
			}
			if (procpar.TryGetValue("p1", out value))
			{
				double p1 = Convert.ToDouble(value[0]);
				if (p1 > 0.0)
					fh.Add("P1", sequel + value[0] + "s");
			}
			if (procpar.TryGetValue("probe_", out value))
				fh.Add("Probe", sequel + value[0]);
			if (procpar.TryGetValue("pw", out value))
			{
				double pw = Convert.ToDouble(value[0]);
				if (pw > 0.0)
					fh.Add("PW", sequel + value[0] + "s");
			}
			if (procpar.TryGetValue("pw90", out value))
			{
				double pw90 = Convert.ToDouble(value[0]);
				if (pw90 > 0.0)
					fh.Add("PW90", sequel + value[0] + "s");
			}
			double f1ref = 100.0;
			if (procpar.TryGetValue("reffrq", out value))
				f1ref = Convert.ToDouble(value[0]);
			if (procpar.TryGetValue("seqfil", out value))
				fh.Add("seq_source", "seq_source\n" + value[0]);
			double f1freq = 100.0;
			if (procpar.TryGetValue("sfrq", out value))
			{
				f1freq = Convert.ToDouble(value[0]);
				hd.Insert(f1freq, 464);
			}
			string solvent = "none";
			if (procpar.TryGetValue("solvent", out value))
			{
				solvent = value[0];
				if (solvent.Length < 1)
					solvent = "none";
				fh.Add("LockSolvent", nl + solvent);
			}
			if (procpar.TryGetValue("spin", out value))
				hd.Insert(Convert.ToInt32(value[0]), 400);
			if (procpar.TryGetValue("ss", out value))
				hd.Insert(Convert.ToInt32(value[0]), 364);
			if (procpar.TryGetValue("sw", out value))
			{
				double sw = Convert.ToDouble(value[0]);
				hd.Insert(sw, 16);
				hd.Insert(1.0 / sw, 80);
			}
			if (procpar.TryGetValue("temp", out value))
				hd.Insert(Convert.ToSingle(value[0]), 872);
			if (procpar.TryGetValue("time_run", out value))
				fh.Add("time_start", nl + value[0]);
			if (procpar.TryGetValue("time_complete", out value))
				fh.Add("time_finish", nl + value[0]);
			string tnstr = "";
			if (procpar.TryGetValue("tn", out value))
				tnstr = value[0];
			if (tnstr.Length < 2)
				tnstr = "off";
			hd.Insert(tnstr, 508);
			if (procpar.TryGetValue("tpwr", out value))
			{
				hd.Insert(Convert.ToInt32(value[0]), 492);
				hd.Insert(1, 488);
			}
			float lpval = 0.0f;
			float rpval = 0.0f;
			if (ftflag > 0)
			{
				if (procpar.TryGetValue("rp", out value))
				{
					rpval = Convert.ToSingle(value[0]);
					hd.Insert(rpval, 192);
					rpval *= 0.0174532925199433f;
				}
				if (procpar.TryGetValue("lp", out value))
				{
					lpval = Convert.ToSingle(value[0]);
					hd.Insert(lpval, 192);
					lpval *= 0.0174532925199433f;
				}
				if (procpar.TryGetValue("lb", out value))
				{
					if (Convert.ToDouble(value[0]) != 0.0)
					{
						string netnmr = "NetNMR\n";
						fh.Add("apo_value", netnmr + value[0]);
						fh.Add("apo_type", netnmr + "Lorentzian");
						fh.Add("apo)units", netnmr + "Hertz");
					}
				}
			}
			if (nosampleinfo)
			{
				file = ReadFile(fbase + @"\text");
				string[] line = file.Split(new char[] {'\n'}, StringSplitOptions.RemoveEmptyEntries);
				for (int i = 0; i < line.Length; i++)
				{
					string vstr = nl + line[i].Trim();
					switch (i)
					{
					case 0:
						fh.Add("sample", vstr);
						break;
					case 1:
						fh.Add("Comment", vstr);
						break;
					case 2:
						fh.Add("user", vstr);
						break;
					case 3:
						fh.Add("workorder", vstr);
						break;
					case 4:
						fh.Add("project", vstr);
						break;
					case 5:
						fh.Add("submitted", vstr);
						break;
					}
				}
			}
			// figure out the reference frequencies and offsets
			// first: try to calculate a lock reference frquency
			// there has to be a f1ref > 0 if solvent == "none"
			// if the solvent != "none" then lockfreq_ will be used
			if (solvent.Equals("none"))
			{
				lref = GetReferenceFrequency(tnstr, 1.0);
				lref = f1ref / lref;
			}
			else
				lref /= 1.0 + 1.0e-6 * GetSolventPPM(solvent);
			// set the reference frequencies
			double f2ref = 0.0;
			string lnstr = "off";
			if (lref > 0.0)
			{
				lnstr = "H2";
				f1ref = GetReferenceFrequency(tnstr, lref);
				if (f2freq > 0.0 && ! dnstr.Equals("off"))
					f2ref = GetReferenceFrequency(dnstr, lref);
			}
			else
			{
				lnstr = "off";
				if (f1ref == 0.0)
					f1ref = f1freq;
				f2ref = f2freq;
			}
			// now put everything in the header
			hd.Insert(lnstr, 848);			// hd.lock.nucleus
			hd.Insert(lref, 784);			// hd.lock.ref_freq
			float of = (float)(1.0e6 * (f1freq - f1ref));
			hd.Insert(f1ref, 48);			// hd.dim_freq[0]
			hd.Insert(of, 176);				// hd.of[0]
			hd.Insert(f1ref, 472);			// hd.f1.ref_freq
			hd.Insert(of / f1ref, 480);		// hd.f1.nu_offset
			of = (float)(1.0e6 * (f2freq - f2ref));
			hd.Insert(f2ref, 56);			// hd.dim_freq[1]
			hd.Insert(of, 180);				// hd.of[1]
			hd.Insert(f2ref, 536);			// hd.f2.ref_freq;
			if (f2ref > 0.0)
				hd.Insert(of / f2ref, 544);	// hd.f2.nu_offset

			//int groupnum = 0;
			//if (array.Length > 0)
			int groupnum = arrayvalue.Length;
			hd.Insert(groupnum, 288);		// hd.groupnum
			if (0 == groupnum)
			{
				hd.Insert(groupnum, 292);	// hd.groupcur
				groupnum = 1;
			}
			string arraystr = nl;
			if (array.Length > 0)
			{
				if (fh.TryGetValue(array, out arraystr))
				{
					string[] typeval = arraystr.Split(new char[] { '\n' });
					arraystr = typeval[0] + "\n";
				}
				else
					arraystr = nl;
			}
			BinaryReader dfile = null;
			if (0 == ftflag)
			{
				FileStream fs = new FileStream(fbase + @"\fid", FileMode.Open, FileAccess.Read);
				dfile = new BinaryReader(fs);
			}
			else
			{
				FileStream fs = new FileStream(fbase + @"\datdir\data", FileMode.Open, FileAccess.Read);
				dfile = new BinaryReader(fs);
			}
			DataFileHeader dfh = new DataFileHeader();
			dfh.Read(dfile); // this will perform network to host byte swapping.
			hd.Insert(dfh.np/2, 236);
			SaneData dt = new SaneData(dfh.np);	// allocate floats for output
			if (2 != dfh.ebytes)
				dfh.ebytes = 4;
			for (int i = 1; i <= groupnum; i++)
			{
				if (groupnum > 1)
					hd.Insert(i, 292);	// hd.groupcur
				if (arrayvalue.Length >= i)
					fh[array] = arraystr + arrayvalue[i - 1];
				hd.Write(stream);
				// write the flex header as one byte array
				string fhstr = "";
				foreach (KeyValuePair<string, string> de in fh)
				{
					string str = de.Key + nl + de.Value + nl;
					fhstr += str.Length.ToString() + nl;
					str = str.Replace('\n', (char)28);
					fhstr += str;
				}
				fhstr += "0\n";
				byte[] bfh = Encoding.ASCII.GetBytes(fhstr);
				for (int ibfh = 0; ibfh < bfh.Length; ibfh++)
				{
					if ((byte)28 == bfh[ibfh])
						bfh[ibfh] = (byte)0;
				}
				stream.Write(bfh, 0, bfh.Length);
				// now comes the data:
				// first skip past the datablockheader
				byte[] dbh = new byte[28];
				dfile.Read(dbh, 0, 28);
				// now read the data, byte reverse it and insert it.
				// SaneHd.Insert will put it back in network order.
				int dsize = dfh.np * dfh.ebytes;
				byte[] dba = new byte[dsize];
				dfile.Read(dba, 0, dsize);
				float[] farray = new float[dfh.np];
				if (2 == dfh.ebytes)
				{
					// data is Int16
					for (int n = 0; n < dfh.np; n++)
					{
						Int16 s = BitConverter.ToInt16(dba, 2 * n);
						s = IPAddress.NetworkToHostOrder(s);
						farray[n] = Convert.ToSingle(s);
					}
				}
				else
				{
					if ((dfh.status & 0x8) > 0)
					{
						for (int n = 0; n < dfh.np; n++)
						{
							Int32 l = BitConverter.ToInt32(dba, 4 * n);
							l = IPAddress.NetworkToHostOrder(l);
							byte[] bl = BitConverter.GetBytes(l);
							farray[n] = BitConverter.ToSingle(bl, 0);
						}
					}
					else
					{
						// data is Int32
						for (int n = 0; n < dfh.np; n++)
						{
							Int32 l = BitConverter.ToInt32(dba, 4 * n);
							l = IPAddress.NetworkToHostOrder(l);
							farray[n] = Convert.ToSingle(l);
						}
					}
				}
				if (ftflag != 0)
				{
					for (i = 0; i < dfh.np; i += 2)
					{
						double dval = rpval + lpval * (double)(dfh.np - i) / (double)(dfh.np);
						double dsin = Math.Sin(dval);
						double dcos = Math.Cos(dval);
						dval = farray[i] * dcos + farray[i + 1] * dsin;
						farray[i + 1] = (float)(farray[i + 1] * dcos - farray[i] * dsin);
						farray[i] = (float)dval;
					}
				}
				for (int n=0; n<dfh.np; n++)
					dt.Insert(farray[n], n);
				dt.Write(stream);
			}
			dfile.Close();
		}

		private double GetSolventPPM(string solvent)
		{
			if (VarianSolventPPM.Count < 1)
			{
				// build the solvent dictionary on first use
				string path = System.Reflection.Assembly.GetExecutingAssembly().CodeBase;
				if (path.StartsWith("file:"))
					path = path.Substring(5);
				path = path.Trim('/').Replace('/', '\\');
				path = Path.GetDirectoryName(path) + @"\solvents.txt";

				string solvents = ReadFile(path);
				string[] line = solvents.Split(new char[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
				for (int i = 0; i < line.Length; i++)
				{
					if ('#' != line[i].Trim()[0])
					{
						string[] word = line[i].Trim().Split(new char[] { }, StringSplitOptions.RemoveEmptyEntries);
						if (word.Length > 1)
							VarianSolventPPM.Add(word[0].ToLower(), Convert.ToDouble(word[1]));
					}
				}
			}
			double ppm;
			if (VarianSolventPPM.TryGetValue(solvent.ToLower(), out ppm))
				return ppm;
			Console.WriteLine(ID + ": Could not fine solvent " + solvent + " in solvents.txt!");
			return 0.0;
		}

		private double GetReferenceFrequency(string nucleus, double lockfrequency)
		{
			if (VarianNucTable.Count < 1)
			{
				// build the nucleus dictionary on first use
				string home = System.Reflection.Assembly.GetExecutingAssembly().CodeBase;
				if (home.StartsWith("file:"))
					home = home.Substring(5);
				home = home.Trim('/').Replace('/', '\\');
				string file = ReadFile(Path.GetDirectoryName(home) + @"\nuctabref.txt");
				string[] line = file.Split(new char[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
				for (int i = 0; i < line.Length; i++)
				{
					if ('#' != line[i].Trim()[0])
					{
						string[] word = line[i].Split(new char[] { }, StringSplitOptions.RemoveEmptyEntries);
						if (word.Length > 1)
						{
							VarianNucTable.Add(word[0], Convert.ToDouble(word[1]));
						}
					}
				}
			}
			double value;
			if (VarianNucTable.TryGetValue(nucleus, out value))
				return lockfrequency * value;
			Console.WriteLine(ID + ": Could not file nucleus " + nucleus + " in nuctabref.txt!");
			return 1.0;
		}

        private void GetBrukerFile(NetworkStream stream, string reqfilename)
        {
			SaneHD hd = new SaneHD(896);
			hd.InsertChar('b', 603);	// f2.hetero_flag
			int i = reqfilename.IndexOf(@"\pdata\");
			string fbase = reqfilename;
			int ftflag = 0;
			if (i > 0)
			{
				ftflag = -1;
				fbase = reqfilename.Substring(0, i);
			}
			hd.Insert(ftflag, 252);
			BitArray cnst = new BitArray(64, false);
			BitArray cpd = new BitArray(8, false);
			BitArray cpdamx = new BitArray(2, false);
			BitArray d = new BitArray(64, false);
			BitArray dl = new BitArray(64, false);
			BitArray l = new BitArray(64, false);
			BitArray p = new BitArray(64, false);
			BitArray pcpd = new BitArray(64, false);
			BitArray pl = new BitArray(64, false);
			BitArray tl = new BitArray(64, false);
			String file = ReadFile(fbase + @"\format.temp");
			foreach (string line in file.Split(new char[] {'\n'})) 
			{
				if (line.Length>5 && 0==line.Substring(0, 6).CompareTo("T_NAME")) 
				{
					string[] tokens = line.Trim().Split(new char[] {' ', '\t'});
					if (tokens.Length > 1) 
					{
						string name = tokens[tokens.Length-1];
                        int vstart = -1;
                        int vsize = 0;
                        bool gotnumber = false;
                        for (int count = 0; count < name.Length; count++)
                        {
                            if (!gotnumber)
                            {
                                if (Char.IsDigit(name[count]))
                                {
                                    if (vstart < 0)
                                        vstart = count;
                                    vsize++;
                                }
                                else
                                {
                                    if (vstart >= 0)
                                        gotnumber = true;
                                }
                            }
                        }
                        if (name.Length > 4 && 0==name.Substring(0, 4).CompareTo("CNST") && 4==vstart)
                            cnst[Convert.ToInt32(name.Substring(vstart, vsize)) % 64] = true;
                        else if (name.Length > 6 && 0 == name.Substring(0, 6).CompareTo("CPDPRG"))
                        {
                            switch (name[6])
                            {
                                case 'B':
                                    cpdamx[0] = true;
                                    break;
                                case 'T':
                                    cpdamx[1] = true;
                                    break;
                                default:
                                    if (6==vstart)
                                        cpd[Convert.ToInt32(name.Substring(vstart, vsize)) % 8] = true;
                                    break;
                            }
                        }
                        else if (name.Length > 2 && 0==name.Substring(0, 2).CompareTo("DL") && 2==vstart)
                            dl[Convert.ToInt32(name.Substring(vstart, vsize)) % 64] = true;
                        else if (name.Length > 1 && 0==name.Substring(0, 1).CompareTo("D") && 1==vstart)
                            d[Convert.ToInt32(name.Substring(vstart, vsize)) % 64] = true;
                        else if (name.Length > 1 && 0==name.Substring(0, 1).CompareTo("L") && 1==vstart)
                            l[Convert.ToInt32(name.Substring(vstart, vsize)) % 64] = true;
                        else if (name.Length > 4 && 0==name.Substring(0, 4).CompareTo("PCPD") && 4==vstart)
                            pcpd[Convert.ToInt32(name.Substring(vstart, vsize)) % 64] = true;
                        else if (name.Length > 2 && 0 == name.Substring(0, 2).CompareTo("PL") && 2==vstart)
                            pl[Convert.ToInt32(name.Substring(vstart, vsize)) % 64] = true;
                        else if (name.Length > 1 && 0==name.Substring(0, 1).CompareTo("P") && 1==vstart)
                            p[Convert.ToInt32(name.Substring(vstart, vsize)) % 64] = true;
                        else if (name.Length > 2 && 0==name.Substring(0, 2).CompareTo("TL") && 2==vstart)
                            tl[Convert.ToInt32(name.Substring(vstart, vsize)) % 64] = true;
					}
				}
			}
			file = fbase + @"\acqu2s";
			int dimension = 1;
			int dim1 = 1;
			if (File.Exists(file)) 
			{
                Dictionary<string, string> acqu2s = new Dictionary<string, string>();
				//BrukerNMRParameters acqu2s = new BrukerNMRParameters();
				MapJCAMPFile(acqu2s, ReadFile(file));
				if (acqu2s.ContainsKey("TD")) 
				{
					dimension = 2;
					dim1 = Convert.ToInt32(acqu2s["TD"]);
					
				}
				acqu2s.Clear();
			}
			hd.Insert(dim1, 240);
			hd.Insert(dimension, 296);
			file = fbase + @"\acqus";
			if (! File.Exists(file))
				return;
			file = ReadFile(file);
             Dictionary<string, string> acqus = new Dictionary<string, string>();
            Dictionary<string, string> fh = new Dictionary<string, string>();
			MapJCAMPFile(acqus, file);
            string value;
			if (acqus.TryGetValue("AQ_mod", out value))
				hd.Insert(Convert.ToInt32(value) + 10, 372);
			if (acqus.TryGetValue("BF1", out value))
				hd.Insert(Convert.ToDouble(value), 48);
			double dim_freq1 = 0.0;
			if (acqus.TryGetValue("BF2", out value))
			{
				dim_freq1 = Convert.ToDouble(value);
				hd.Insert(dim_freq1, 56);
			}
			if (cpdamx[0]) 
			{
				if (acqus.TryGetValue("CPDPRGB", out value))
					fh.Add("CPDPRGB", "sequel\n"+value);
			}
			if (cpdamx[1]) 
			{
				if (acqus.TryGetValue("CPDPRGT", out value))
					fh.Add("CPDPRGT", "sequel\n"+value);
			}
			for (i=0; i<8; i++) 
			{
				if (cpd[i]) 
				{
					string pname = "CPDPRG" + i.ToString();
					if (acqus.TryGetValue(pname, out value))
					{
						string vstr = value;
						if (vstr.Length > 0) 
						{
							fh.Add(pname, "sequel\n" + vstr);
							if (i == 2) 
							{
								if (0 <= vstr.IndexOf("mlev"))
									hd.Insert((int)1, 572);
								else if (0 <= vstr.IndexOf("waltz"))
									hd.Insert((int)3, 572);
								else if (0 <= vstr.IndexOf("garp"))
									hd.Insert((int)9, 572);
								else if (0 <= vstr.IndexOf("bb"))
									hd.Insert((int)8, 572);
								else if (0 <= vstr.IndexOf("tppm"))
									hd.Insert((int)12, 572);
								else
									hd.Insert((int)999, 572);
							}
						}
					}
				}
				if (acqus.TryGetValue("PCPD2", out value) && pcpd[2]) 
				{
					float x = Convert.ToSingle(value);
					if (x > 0.0) 
					{
						x = 2.5e5F/x;
						hd.Insert(x, 552);
					}
				}
			}
			int bytord = 1;
			if (acqus.TryGetValue("BYTORDA", out value))
				bytord = Convert.ToInt32(value);
			SequelArray(acqus, fh, cnst, "CNST");
			SequelArray(acqus, fh, d, "D");
			if (acqus.TryGetValue("DATE", out value))
			{
				long time = Convert.ToInt64(value);
				DateTime ftime = new DateTime(1970, 1, 1);
				ftime = ftime.Add(new TimeSpan(time*10000000L));
				ftime = TimeZone.CurrentTimeZone.ToLocalTime(ftime);
				fh.Add("date", "\n"+ftime.ToString());
			}
			SequelArray(acqus, fh, dl, "DL");
			int decim = 0;
			if (acqus.TryGetValue("DECIM", out value))
				decim = Convert.ToInt32(value);
			if (acqus.TryGetValue("DS", out value))
				hd.Insert(Convert.ToInt32(value), 364);
			int dspfvs = 0;
			if (acqus.TryGetValue("DSPFVS", out value))
				dspfvs = Convert.ToInt32(value);
            float grpdly = 0f;
            if (acqus.TryGetValue("GRPDLY", out value))
                grpdly = Convert.ToSingle(value);
			SequelArray(acqus, fh, l, "L");
			if (acqus.TryGetValue("LGAIN", out value))
				hd.Insert(Convert.ToInt32(Convert.ToSingle(value)), 812);
			if (acqus.TryGetValue("LOCKPOW", out value))
				hd.Insert(Convert.ToInt32(Convert.ToSingle(value)), 816);
			if (acqus.TryGetValue("LOCNUC", out value))
				hd.Insert(value, 848);
			if (acqus.TryGetValue("NS", out value))
				hd.Insert(Convert.ToInt32(value), 356);
			if (acqus.TryGetValue("NUCLEUS", out value)) 
			{
				string nucleus = value;
				if (0 == nucleus.CompareTo("off") && acqus.TryGetValue("NUC1", out value))
					nucleus = value;
				hd.Insert(nucleus, 508);
			}
			if (acqus.TryGetValue("DECNUC", out value)) 
			{
				string nucleus = value;
				if (0==nucleus.CompareTo("off") && acqus.TryGetValue("NUC2", out value))
					nucleus = value;
				hd.Insert(nucleus, 604);
			}
			if (acqus.TryGetValue("O1", out value))
				hd.Insert(Convert.ToSingle(value), 176);
			if (acqus.TryGetValue("O2", out value)) 
			{
				float of1 = Convert.ToSingle(value);
				hd.Insert(of1, 180);
				if (dim_freq1 != 0.0)
					hd.Insert(of1/dim_freq1, 544);
			}
			SequelArray(acqus, fh, p, "P");
			SequelArray(acqus, fh, pl, "PL");
			if (acqus.TryGetValue("PROBHD", out value))
				fh.Add("Probe", "sequel\n" + value.Trim());
			if (acqus.TryGetValue("PULPROG", out value))
				fh.Add("seq_source", "seq_source\n" + value);
			if (acqus.TryGetValue("RG", out value))
				hd.Insert(Convert.ToSingle(value), 752);
			if (acqus.TryGetValue("SFO1", out value))
				hd.Insert(Convert.ToDouble(value), 464);
			if (acqus.TryGetValue("SFO2", out value))
				hd.Insert(Convert.ToDouble(value), 528);
            if (acqus.TryGetValue("SOLVENT", out value))
            {
                if (value.Trim().Length > 0)
                    fh.Add("LockSolvent", "\n" + value.Trim());
            }
			double dwelltime = 0.0;
			if (acqus.TryGetValue("SW_h", out value)) 
			{
				double sw = Convert.ToDouble(value);
				hd.Insert(sw, 16);
				if (sw != 0.0) 
				{
					dwelltime = 1.0/sw;
					hd.Insert(dwelltime, 80);
				}
			}
			if (acqus.TryGetValue("TE", out value))
			{
				hd.Insert(Convert.ToSingle(value)-273.15F, 872);
				hd.Insert((int)1, 888);
			}
			int npoints = 0;
			if (acqus.TryGetValue("TD", out value)) 
			{
				npoints = Convert.ToInt32(value) / 2;
				hd.Insert(npoints, 236);
				hd.Insert(npoints, 284);
			}
			SequelArray(acqus, fh, tl, "TL");
			string arrayname = "";
			string[] array = null;
			if (2 == dimension)
			{
                string fname = fbase + @"\difflist";
                if (File.Exists(fname))
                {
                    arrayname = "DiffGradient";
                    array = ReadFile(fname).Split(new char[] { '\n' });
                    fname = reqfilename + @"\dosy";
                    if (0 == ftflag)
                    {
                        if (Directory.Exists(fbase + @"\pdata"))
                        {
                            foreach (string procno in NumberDirs(fbase + @"\pdata"))
                            {
                                fname = fbase + @"\pdata\" + procno + @"\dosy";
                                if (File.Exists(fname))
                                    break;
                            }
                        }
                    }
                    if (File.Exists(fname))
                    {
                        //BrukerNMRParameters dosy = new BrukerNMRParameters();
                        Dictionary<string, string> dosy = new Dictionary<string, string>();
                        MapJCAMPFile(dosy, ReadFile(fname));
                        if (dosy.TryGetValue("Gamma", out value))
                        {
                            double gamma = Convert.ToDouble(value);
                            gamma *= 62831.853;
                            fh.Add("dosygamma", "DOSY\n" + Convert.ToString(gamma));
                        }
                        double timecubed = 0.0;
                        string gdist, glen;
                        if (dosy.TryGetValue("Gdist", out gdist) && dosy.TryGetValue("Glen", out glen))
                        {
                            timecubed = Convert.ToDouble(glen);
                            timecubed *= timecubed;
                            timecubed *= Convert.ToDouble(gdist);
                            timecubed *= 1.0e-9;
                        }
                        fh.Add("timecubed", "DOSY\n" + Convert.ToString(timecubed));

                        dosy.Clear();
                    }
                    else
                    {
                        fh.Add("dosygamma", "DOSY\n267520000");
                        double timecubed = 1.0;
                        string p30, d20;
                        if (fh.TryGetValue("P30", out p30) && fh.TryGetValue("D20", out d20))
                        {
                            string[] values = p30.Split(new Char[] {'\n'});
                            double littled = 2.0 * Convert.ToDouble(values[1].Trim(new Char[] {'u', 'm', 's'}));
                            if (values[1].EndsWith("us"))
                                littled *= 1.0e-6;
                            else if (values[1].EndsWith("ms"))
                                littled *= 1.0e-3;
                            values = d20.Split(new Char[] {'\n'});
                            double bigd = Convert.ToDouble(values[1].Trim(new Char[] {'u', 'm', 's'}));
                            if (values[1].EndsWith("us"))
                                bigd *= 1.0e-6;
                            else if (values[1].EndsWith("ms"))
                                bigd *= 1.0e-3;
                            timecubed = littled * littled * (bigd - littled / 3.0);
                        }
                        fh.Add("timecubed", "DOSY\n" + Convert.ToString(timecubed));
                    }
                    fh.Add("DAC_to_G", "DOSY\n1.0");
                }
                else
                {
                    foreach (string lname in new string[] { "VDLIST", "VPLIST", "VCLIST", "VTLIST" })
                    {
                        if (acqus.ContainsKey(lname))
                        {
                            fname = acqus[lname];
                            if (1 > fname.Length)
                                fname = "X";
                            fname = fbase + @"\" + fname;
                            if (File.Exists(fname))
                                array = ReadFile(fname).Split(new char[] { '\n' });
                            else
                            {
                                fname = fbase + @"\" + lname.ToLower();
                                if (File.Exists(fname))
                                    array = ReadFile(fname).Split(new char[] { '\n' });
                            }
                            if (array != null)
                            {
                                arrayname = lname.Substring(0, 2);
                                break;
                            }
                        }
                    }
                }
			}
			hd.Insert(dwelltime*npoints, 112);
			double norcon = 0.0;
            if (ftflag != 0)
            {
                file = reqfilename + @"\procs";
                file = ReadFile(file);
                if (file.Length > 0)
                {
                    Dictionary<string, string> procs = new Dictionary<string, string>();
                    MapJCAMPFile(procs, file);
                    if (procs.TryGetValue("BYTORDP", out value))
                        bytord = Convert.ToInt32(value);
                    if (procs.TryGetValue("FTSIZE", out value))
                    {
                        npoints = Convert.ToInt32(value);
                        hd.Insert(npoints, 236);
                    }
                    if (procs.TryGetValue("NC_proc", out value))
                        norcon = Convert.ToDouble(value);
                    if (procs.TryGetValue("WDW", out value))
                    {
                        i = Convert.ToInt32(value);
                        if (i == 1)
                        {
                            if (procs.TryGetValue("LB", out value))
                            {
                                fh.Add("apo_type", "\nLorentzian");
                                fh.Add("apo_value", "\n" + value);
                                fh.Add("apo_unit", "\nHertz");
                            }
                        }
                        else if (i == 2)
                        {
                            if (procs.TryGetValue("GM", out value))
                            {
                                fh.Add("apo_type", "\nGaussian");
                                fh.Add("apo_value", "\n" + value);
                                fh.Add("apo_unit", "\nHertz");
                            }
                        }
                    }
                }
            }
            else
            {
                if (grpdly > 0f)
                    hd.Insert(360f * grpdly, 220);
                else
                    hd.Insert(LookupPhase(decim, dspfvs), 220);
            }
			file = fbase + @"\sample_info.prop";
			file = ReadFile(file);
			if (file.Length > 0) 
				MapPropFile(fh, file);
			else
			{
				file = fbase + @"\info";
				file = ReadFile(file);
				if (file.Length > 0)
					MapInfoFile(fh, file);
				else
				{
					file = ReadFile(reqfilename + @"\title");
					if (0 == file.Length)
						file = ReadFile(reqfilename + @"\pdata\1\title");
					if (file.Length > 0)
					{
						string[] lines = file.Split(new char[] {'\n'});
						if (lines.Length > 0) 
						{
							fh["sample"] = "\n" + lines[0].Trim();
							if (lines.Length > 1) 
							{
								fh["comment"] = "\n" + lines[1].Trim();
								if (lines.Length > 2)
								{
									fh["user"] = "\n" + lines[2].Trim();
									if (lines.Length > 3) 
									{
										fh["workorder"] = "\n" + lines[3].Trim();
										if (lines.Length > 4)
										{
											fh["project"] = "\n" + lines[4].Trim();
                                            if (lines.Length > 5)
                                            {
                                                fh["submitted"] = "\n" + lines[5].Trim();
                                                if (lines.Length > 6)
                                                    fh["dataclass"] = "\n" + lines[6].Trim();
                                            }
										}
									}
								}
							}
						}
					}
				}
			}
			if (dim1>1 && dimension==2 && arrayname.Length > 0)
			{
				fh["array"] = "\n" + arrayname;
				hd.Insert(dim1, 288);	// hd.groupnum
			}
			string sext, iext, rext;
			if (1 == dimension)
			{
				sext = @"\fid";
				rext = @"\1r";
				iext = @"\1i";
			}
			else
			{
				sext = @"\ser";
				rext = @"\2rr";
				iext = @"\2ii";
			}
			BinaryReader sfile = null;
			BinaryReader rfile = null;
			BinaryReader ifile = null;
			if (0 == ftflag)
			{
				FileStream fs = new FileStream(reqfilename+sext, FileMode.Open, FileAccess.Read);
				sfile = new BinaryReader(fs);
			}
			else
			{
				FileStream rfs = new FileStream(reqfilename+rext, FileMode.Open, FileAccess.Read);
				rfile = new BinaryReader(rfs);
				FileStream ifs = new FileStream(reqfilename+iext, FileMode.Open, FileAccess.Read);
				ifile = new BinaryReader(ifs);
			}
			float fnorm = Convert.ToSingle(Math.Pow(2.0, norcon));
			SaneData dt = new SaneData(2*npoints);
			for (int spec=0; spec<dim1; spec++)
			{
                // first, write the sane (fixed) header
				if (dim1 > 1)
				{
					hd.Insert(spec+1, 292);		//hd.groupcur
					if (spec<array.Length)
						fh[arrayname] = "\n" + array[spec].Trim();
					else
						fh[arrayname] = "\nIndex=" + spec.ToString();
				}
				hd.Write(stream);

                // write the flexheader in one stream.write
                String flexheader = "";
 				foreach (KeyValuePair<string, string> de in fh)
				{
					String str = de.Key + "\n" + de.Value + "\n";
					int len = str.Length;
                    flexheader += len.ToString() + "\n";
					//WriteLine(stream, len.ToString());
                    str = str.Replace('\n', (char)28);	// these key, type and value strings will be terminated with a null
                    flexheader += str;
				}
                flexheader += "0\n";
                byte[] bfh = Encoding.ASCII.GetBytes(flexheader);
                for (int ibfh = 0; ibfh < bfh.Length; ibfh++)
                    if ((byte)28 == bfh[ibfh])
                        bfh[ibfh] = (byte)0;
                stream.Write(bfh, 0, bfh.Length);

                // now the data
				if(0 == ftflag)
				{
					if (bytord == 0)
					{
						for (i=0; i<npoints; i++)
						{
							dt.Insert(Convert.ToSingle(sfile.ReadInt32()), 2*i);
							dt.Insert(Convert.ToSingle(-sfile.ReadInt32()), 2*i+1);
						}
					}
					else
					{
						for (i=0; i<npoints; i++)
						{
							dt.Insert(Convert.ToSingle(IPAddress.NetworkToHostOrder(sfile.ReadInt32())), 2*i);
							dt.Insert(Convert.ToSingle(-IPAddress.NetworkToHostOrder(sfile.ReadInt32())), 2*i+1);
						}
					}
				}
				else
				{
					if (bytord == 0)
					{
						for (i=0; i<npoints; i++)
						{
							dt.Insert(Convert.ToSingle(rfile.ReadInt32())*fnorm, 2*i);
							dt.Insert(Convert.ToSingle(ifile.ReadInt32())*fnorm, 2*i+1);
						}
					}
					else
					{
						for (i=0; i<npoints; i++)
						{
							dt.Insert(Convert.ToSingle(IPAddress.NetworkToHostOrder(rfile.ReadInt32()))*fnorm, 2*i);
							dt.Insert(Convert.ToSingle(IPAddress.NetworkToHostOrder(ifile.ReadInt32()))*fnorm, 2*i+1);
						}
					}
				}
				dt.Write(stream);
			}
			if (0 == ftflag)
				sfile.Close();
			else
			{
				rfile.Close();
				ifile.Close();
			}
		}

		void SequelArray(Dictionary<string,string> acqus, Dictionary<string,string> fh, BitArray flag, string name)
		{
			for (int i=0; i<flag.Length; i++) 
			{
				if (flag[i]) 
				{
					string param = name + i.ToString();
					string val, value;
					if (acqus.TryGetValue(param, out value))
					{
						float x = Convert.ToSingle(value);
						if (0 == name.CompareTo("D")) 
						{
							if (x < 1.0e-3F)
							{
								x *= 1.0e6F;
								val = "sequel\n" + x.ToString() + "us";
							}
							else if (x < 1.0F)
							{
								x *= 1.0e3F;
								val = "sequel\n" + x.ToString() + "ms";
							}
							else
								val = "sequel\n"+ x.ToString() + "s";
						} 
						else if (0 == name.CompareTo("P"))
						{
							if (x > 1.0e6F)
							{
								x *= 1.0e-6F;
								val = "sequel\n" + x.ToString() + "s";
							}
							else if (x > 1.0e3F)
							{
								x *= 1.0e-3F;
								val = "sequel\n" + x.ToString() + "ms";
							}
							else
								val = "sequel\n" + x.ToString() + "us";
						}
						else
							val = "sequel\n" + x.ToString();
						fh.Add(param, val);
					}
				}
			}
		}
		float LookupPhase(int decim, int dspfvs)
		{
			switch (dspfvs)
			{
			case 10:
				switch (decim)
				{
				case 2:
					return (float)(179.0 * 180.0 / decim);
				case 3:
					return (float)(201.0 * 180.0 / decim);
				case 4:
					return (float)(533.0 * 180.0 / decim);
				case 6:
					return (float)(709.0 * 180.0 / decim);
				case 8:
					return (float)(1097.0 * 180.0 / decim);
				case 12:
					return (float)(1449.0 * 180.0 / decim);
				case 16:
					return (float)(2225.0 * 180.0 / decim);
				case 24:
					return (float)(2929.0 * 180.0 / decim);
				case 32:
					return (float)(4481.0 * 180.0 / decim);
				case 48:
					return (float)(5889.0 * 180.0 / decim);
				case 64:
					return (float)(8993.0 * 180.0 / decim);
				case 96:
					return (float)(11809.0 * 180.0 / decim);
				case 128:
					return (float)(18017.0 * 180.0 / decim);
				case 192:
					return (float)(23649.0 * 180.0 / decim);
				case 256:
					return (float)(36065.0 * 180.0 / decim);
				case 384:
					return (float)(47329.0 * 180.0 / decim);
				case 512:
					return (float)(72161.0 * 180.0 / decim);
				case 768:
					return (float)(94689.0 * 180.0 / decim);
				case 1024:
					return (float)(144353.0 * 180.0 / decim);
				case 1536:
					return (float)(189409.0 * 180.0 / decim);
				case 2048:
					return (float)(288737.0 * 180.0 / decim);
				}
				break;
			case 11:
				switch (decim)
				{
				case 2:
					return (float)(184.0 * 180.0 / decim);
				case 3:
					return (float)(219.0 * 180.0 / decim);
				case 4:
					return (float)(384.0 * 180.0 / decim);
				case 6:
					return (float)(602.0 * 180.0 / decim);
				case 8:
					return (float)(852.0 * 180.0 / decim);
				case 12:
					return (float)(1668.0 * 180.0 / decim);
				case 16:
					return (float)(2312.0 * 180.0 / decim);
				case 24:
					return (float)(3368.0 * 180.0 / decim);
				case 32:
					return (float)(4656.0 * 180.0 / decim);
				case 48:
					return (float)(6768.0 * 180.0 / decim);
				case 64:
					return (float)(9344.0 * 180.0 / decim);
				case 96:
					return (float)(13568.0 * 180.0 / decim);
				case 128:
					return (float)(18560.0 * 180.0 / decim);
				case 192:
					return (float)(27392.0 * 180.0 / decim);
				case 256:
					return (float)(36992.0 * 180.0 / decim);
				case 384:
					return (float)(55040.0 * 180.0 / decim);
				case 512:
					return (float)(73856.0 * 180.0 / decim);
				case 768:
					return (float)(110336.0 * 180.0 / decim);
				case 1024:
					return (float)(147584.0 * 180.0 / decim);
				case 1536:
					return (float)(220928.0 * 180.0 / decim);
				case 2048:
					return (float)(295040.0 * 180.0 / decim);
				}
				break;
			case 12:
				switch (decim)
				{
				case 2:
					return (float)(184.0 * 180.0 / decim);
				case 3:
					return (float)(219.0 * 180.0 / decim);
				case 4:
					return (float)(384.0 * 180.0 / decim);
				case 6:
					return (float)(602.0 * 180.0 / decim);
				case 8:
					return (float)(852.0 * 180.0 / decim);
				case 12:
					return (float)(1668.0 * 180.0 / decim);
				case 16:
					return (float)(2292.0 * 180.0 / decim);
				case 24:
					return (float)(3368.0 * 180.0 / decim);
				case 32:
					return (float)(4616.0 * 180.0 / decim);
				case 48:
					return (float)(6768.0 * 180.0 / decim);
				case 64:
					return (float)(9264.0 * 180.0 / decim);
				case 96:
					return (float)(13568.0 * 180.0 / decim);
				case 128:
					return (float)(18560.0 * 180.0 / decim);
				case 192:
					return (float)(27392.0 * 180.0 / decim);
				case 256:
					return (float)(36992.0 * 180.0 / decim);
				case 384:
					return (float)(55040.0 * 180.0 / decim);
				case 512:
					return (float)(73856.0 * 180.0 / decim);
				case 768:
					return (float)(110336.0 * 180.0 / decim);
				case 1024:
					return (float)(147584.0 * 180.0 / decim);
				case 1536:
					return (float)(220928.0 * 180.0 / decim);
				case 2048:
					return (float)(295040.0 * 180.0 / decim);
				}
				break;
			case 13:
				switch (decim)
				{
				case 2:
					return (float)(11.0 * 180.0 / decim);
				case 3:
					return (float)(17.0 * 180.0 / decim);
				case 4:
					return (float)(23.0 * 180.0 / decim);
				case 6:
					return (float)(35.0 * 180.0 / decim);
				case 8:
					return (float)(47.0 * 180.0 / decim);
				case 12:
					return (float)(71.0 * 180.0 / decim);
				case 16:
					return (float)(95.0 * 180.0 / decim);
				case 24:
					return (float)(143.0 * 180.0 / decim);
				case 32:
					return (float)(191.0 * 180.0 / decim);
				case 48:
					return (float)(287.0 * 180.0 / decim);
				case 64:
					return (float)(383.0 * 180.0 / decim);
				case 96:
					return (float)(575.0 * 180.0 / decim);
				}
				break;
			}
			return (float)0.0;
		}

		private void GetSpectrum(NetworkStream stream, bool Is2D, string reqfilename)
		{
			if (reqfilename.EndsWith(".pro"))
				GetVarianSpectrum(stream, Is2D, reqfilename);
			else
				GetBrukerSpectrum(stream, Is2D, reqfilename);
		}

		private void GetVarianSpectrum(NetworkStream stream, bool Is2D, string reqfilename)
		{
			string fbase = reqfilename.Substring(0, reqfilename.Length - 3) + "fid";
			string file = ReadFile(fbase + @"\procpar");
			Dictionary<string, string[]> procpar = new Dictionary<string, string[]>();
			MapProcparFile(procpar, file);
			double lfrq;
			string[] nuc = new string[2];
			string dm, sample, comment, dataclass, seqfil, solvent, refsource;
			string[] value;
			double f1frq, f1ref, f2frq, f2ref;
			float[] sw = new float[2];
			if (procpar.TryGetValue("dfrq", out value))
				f2frq = Convert.ToDouble(value[0]);
			else
				f2frq = 0.0;
			if (procpar.TryGetValue("dm", out value))
				dm = value[0];
			else
				dm = "nnn";
			if (procpar.TryGetValue("dn", out value))
				nuc[1] = value[0];
			else
				nuc[1] = "";
			if (procpar.TryGetValue("gesample", out value))
			{
				sample = value[0];
				if (procpar.TryGetValue("gecomment", out value))
					comment = value[0];
				else
					comment = "";
				if (procpar.TryGetValue("gedataclass", out value))
					dataclass = value[0];
				else
					dataclass = "";
			}
			else
			{
				// no gesample found in procpar, try to get sample and comment from the text file.
				file = ReadFile(fbase + @"\text");
				string[] line = file.Split(new char[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
				if (line.Length > 0)
				{
					sample = line[0].Trim();
					if (line.Length > 1)
						comment = line[1].Trim();
					else
						comment = "";
				}
				else
					comment = sample = "";
				dataclass = "";
			}
			if (procpar.TryGetValue("lockfreq_", out value))
				lfrq = Convert.ToDouble(value[0]);
			else
				lfrq = 0.0;
			if (procpar.TryGetValue("reffrq", out value))
				f1ref = Convert.ToDouble(value[0]);
			else
				f1ref = 100.0;
			if (procpar.TryGetValue("reffrq1", out value))
				f2ref = Convert.ToDouble(value[0]);
			else
				f2ref = 100.0;
			if (procpar.TryGetValue("seqfil", out value))
				seqfil = value[0];
			else
				seqfil = "";
			if (procpar.TryGetValue("sfrq", out value))
				f1frq = Convert.ToDouble(value[0]);
			else
				f1frq = 0.0;
			if (procpar.TryGetValue("solvent", out value))
				solvent = value[0];
			else
				solvent = "none";
			if (procpar.TryGetValue("sw", out value))
				sw[0] = Convert.ToSingle(value[0]);
			else
				sw[0] = 1000.0f;
			if (procpar.TryGetValue("sw1", out value))
				sw[1] = Convert.ToSingle(value[0]);
			else
				sw[1] = 1000.0f;
			if (procpar.TryGetValue("tn", out value))
				nuc[0] = value[0];
			else
				nuc[0] = "";
			if (procpar.TryGetValue("refsource1", out value))
				refsource = value[0];
			else
				refsource = "sfrq";

			// now sort out the spectral window(s)
			if (solvent.Equals("none"))
			{
				lfrq = GetReferenceFrequency(nuc[0], 1.0);
				lfrq = f1ref / lfrq;
			} else
				lfrq /= 1.0 + 1.0e-6 * GetSolventPPM(solvent);
			if (lfrq > 0.0)
			{
				f1ref = GetReferenceFrequency(nuc[0], lfrq);
				if (refsource.Equals("dfrq"))
					f2ref = GetReferenceFrequency(nuc[1], lfrq);
				else
					f2ref = f1ref;
			}
			else
			{
				f1ref = f1frq;
				if (refsource.Equals("dfrq"))
					f2ref = f2frq;
				else
					f2ref = f2frq;
			}
			float[] sf = new float[2];
			sf[0] = (float)f1ref;
			sf[1] = (float)f2ref;
			float[] of = new float[2];
			of[0] = (float)((1.0e6 * (f1frq - f1ref) + 0.5 * sw[0]) / f1ref);
			if (refsource.Equals("dfrq"))
				of[1] = (float)((1.0e6 * (f2frq - f2ref) + 0.5 * sw[1]) / f2frq);
			else
				of[1] = of[0];
			// write the spectrum to stream
			if (sample.Length > 0)
				WriteLine(stream, sample);
			if (comment.Length > 0)
				WriteLine(stream, comment);
			string buffer = "";
			if (seqfil.Length > 0)
				buffer += seqfil + " - ";
			if (nuc[0].Length > 0)
				buffer += nuc[0];
			if (nuc[1].Length > 0)
				buffer += "{" + nuc[1] + "}";
			WriteLine(stream, buffer);
			if (dataclass.Length > 0)
				WriteLine(stream, "dataclass=" + dataclass);
			WriteLine(stream, ".");

			// time to open the phasefile
			string phasefile = fbase + @"\datdir\phasefile";
			FileInfo finfo = new FileInfo(phasefile);
			Int64 fsize = finfo.Length;
			BinaryReader br = new BinaryReader(new FileStream(phasefile, FileMode.Open, FileAccess.Read));
			DataFileHeader dfh = new DataFileHeader();
			dfh.Read(br);
			if ((dfh.status & 0x33) != 0x3)
			{
				Console.WriteLine(ID + ": Data type not supported, status = " + dfh.status.ToString());
				return;	// data is not a simple spectrum
			}
			if (2 != dfh.ebytes)
				dfh.ebytes = 4;
			int[] np = new int[2];
			if (Is2D)
			{
				np[0] = dfh.nblocks * dfh.ntraces;
				np[1] = dfh.np;
				if (np[0] < 2)
				{
					Console.WriteLine(ID + ": unexpected number of traces for 2D file, np[0] = " + np[0].ToString());
					return;	// unexpected value
				}
			}
			else
			{
				np[0] = dfh.np;
				np[1] = dfh.nblocks * dfh.ntraces;
				if (np[1] != 1)
				{
					Console.WriteLine(ID + ": unexpected number of traces for 1D file, np[1] = " + np[1].ToString());
					return;	// unexpected value
				}
			}
			SaneHD hd = new SaneHD(36);
			int dim = 1;
			if (Is2D)
				dim = 2;
			hd.Insert(dim, 0);
			for (int i = 0; i < dim; i++)
			{
				hd.Insert(np[i], 4 * (1 + i * 4));
				hd.Insert(sf[i], 4 * (2 + i * 4));
				hd.Insert(sw[i], 4 * (3 + i * 4));
				hd.Insert(of[i], 4 * (4 + i * 4));
			}
			hd.Write(stream);
			Int32[,] idata = new Int32[dfh.ntraces*dfh.nblocks, dfh.np];
			byte[] bbyte = new byte[dfh.bbytes];

			if (fsize == 32 + 2 * dfh.nblocks * dfh.bbytes)
			{
				// the file was saved with f2 along the horizontal
				for (int i=0; i<dfh.nblocks; i++)
					br.Read(bbyte, 0, dfh.bbytes);
			}
			else
				fsize = 0;
			if ((dfh.status & 0x8) > 0)
			{
				// data is float (in Network order)
				// read it in to idata as Int32 (in Host order)
				int m = 0;
				int n = 0;
				for (int i = 0; i < dfh.nblocks; i++)
				{
					br.Read(bbyte, 0, dfh.bbytes);
					for (int j = 28 * dfh.nbheaders; j < dfh.bbytes; j += dfh.ebytes)
					{
						idata[m, n++] = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(bbyte, j));
						if (n >= dfh.np)
						{
							n = 0;
							m++;
						}
					}
				}
				// now check for max
				float fmax = 0.0f;
				for (int i = 0; i < dfh.nblocks * dfh.ntraces; i++)
				{
					for (int j = 0; j < dfh.np; j++)
					{
						float f = BitConverter.ToSingle(BitConverter.GetBytes(idata[i, j]), 0);
						if (f > fmax)
							fmax = f;
					}
				}
				fmax = 1073741824.0f / fmax;
				// normalize and restore ints back (in Network order)
				for (int i = 0; i < dfh.nblocks * dfh.ntraces; i++)
				{
					for (int j = 0; j < dfh.np; j++)
					{
						idata[i, j] = IPAddress.HostToNetworkOrder(Convert.ToInt32(fmax * BitConverter.ToSingle(BitConverter.GetBytes(idata[i, j]), 0)));
					}
				}
			}
			else
			{
				if ((dfh.status & 0x4) > 0)
				{
					// data is Int32 (in Network order)
					// so no byte swapping is necessary!
					int m = 0;
					int n = 0;
					for (int i = 0; i < dfh.nblocks; i++)
					{
						br.Read(bbyte, 0, dfh.bbytes);
						for (int j = 2828 * dfh.nbheaders; j < dfh.bbytes; j += dfh.ebytes)
						{
							idata[m, n++] = BitConverter.ToInt32(bbyte, j);
							if (n >= dfh.np)
							{
								n = 0;
								m++;
							}
						}
					}
				}
				else
				{
					// data is Int16
					int m = 0;
					int n = 0;
					for (int i = 0; i < dfh.nblocks; i++)
					{
						br.Read(bbyte, 0, dfh.bbytes);
						for (int j = 2828 * dfh.nbheaders; j < dfh.bbytes; j += dfh.ebytes)
						{
							idata[m, n++] = IPAddress.HostToNetworkOrder(Convert.ToInt32(IPAddress.NetworkToHostOrder(BitConverter.ToInt16(bbyte, j))));
							if (n >= dfh.np)
							{
								n = 0;
								m++;
							}
						}
					}
				}
			}
			// write the data to the stream
			if (Is2D)
			{
				if (0 == fsize)
				{
					for (int j = 0; j < np[0]; j++)
						for (int i = 0; i < np[1]; i++)
							stream.Write(BitConverter.GetBytes(idata[i, j]), 0, sizeof(Int32));
				}
				else
				{
					for (int i = 0; i < np[0]; i++)
						for (int j = 0; j < np[1]; j++)
							stream.Write(BitConverter.GetBytes(idata[i, j]), 0, sizeof(Int32));
				}
			}
			else
			{	// it's a 1D file
				for (int i = 0; i < np[0]; i++)
					stream.Write(BitConverter.GetBytes(idata[0, i]), 0, sizeof(Int32));
			}
		}

		private void GetBrukerSpectrum(NetworkStream stream, bool Is2D, string reqfilename)
		{
			int i = reqfilename.IndexOf(@"\pdata\");
			string fbase = reqfilename.Substring(0,i);
			String file = fbase + @"\acqus";
			file = ReadFile(file);
			string pulprog = "";
			string nuc1 = "";
			string nuc2 = "";
            string value;
			if (file.Length > 0)
			{
				Dictionary<string,string> acqus = new Dictionary<string,string>();
				MapJCAMPFile(acqus, file);
				if (acqus.TryGetValue("NUC1", out value))
					nuc1 = value;
				if (acqus.TryGetValue("NUC2", out value))
					nuc2 = value;
				if (acqus.TryGetValue("PULPROG", out value))
					pulprog = value;
			}
			string commentsamp = "";
			string commentcom = "";
            string commentdataclass = "";
			file = fbase + @"\sample_info.prop";
			file = ReadFile(file);
			if (file.Length > 0) 
			{
				Dictionary<string,string> prop = new Dictionary<string,string>();
				MapPropFile(prop, file);
				if (prop.TryGetValue("sample", out value))
					commentsamp = value.Trim();
				if (prop.TryGetValue("comment", out value))
					commentcom = value.Trim();
                if (prop.TryGetValue("dataclass", out value))
                    commentdataclass = value.Trim();
			}
			else
			{
				file = fbase + @"\info";
				file = ReadFile(file);
				if (file.Length > 0)
				{
                    Dictionary<string, string> info = new Dictionary<string, string>();
					MapInfoFile(info, file);
					if (info.TryGetValue("sample", out value))
						commentsamp = value.Trim();
					if (info.TryGetValue("comment", out value))
						commentcom = value.Trim();
                    if (info.TryGetValue("dataclass", out value))
                        commentdataclass = value.Trim();
				}
				else
				{
					file = reqfilename + @"\title";
					file = ReadFile(file);
					if (file.Length > 0)
					{
						string[] lines = file.Split(new char[] {'\n'});
						if (lines.Length > 0)
						{
							commentsamp = lines[0].Trim();
							if (lines.Length > 1)
							{
								commentcom = lines[1].Trim();
                                if (lines.Length > 2)
                                {
                                    commentdataclass = lines[2].Trim();
                                }
							}
						}
					}
				}
			}
			int bytord = 0;
			int ncproc = 0;
			int []np = new int[2];
			float[] of = new float[2];
			float[] sf = new float[2];
			float[] sw = new float[2];
			file = reqfilename + @"\procs";
			file = ReadFile(file);
			if (file.Length > 0)
			{
                Dictionary<string, string> procs = new Dictionary<string, string>();
				MapJCAMPFile(procs, file);
				if (procs.TryGetValue("BYTORDP", out value))
					bytord = Convert.ToInt32(value);
				if (procs.TryGetValue("FTSIZE", out value))
					np[0] = Convert.ToInt32(value);
				if (procs.TryGetValue("OFFSET", out value))
					of[0] = Convert.ToSingle(value);
				if (procs.TryGetValue("SF", out value))
					sf[0] = Convert.ToSingle(value);
				if (procs.TryGetValue("SW_p", out value))
					sw[0] = Convert.ToSingle(value);
				if (procs.TryGetValue("NC_proc", out value))
					ncproc = Convert.ToInt32(value);
			}
			if (Is2D) {
				file = reqfilename + @"\proc2s";
				file = ReadFile(file);
				if (file.Length > 0)
				{
                    Dictionary<string, string> proc2s = new Dictionary<string, string>();
					MapJCAMPFile(proc2s, file);
					if (proc2s.TryGetValue("FTSIZE", out value))
						np[1] = Convert.ToInt32(value);
					if (0 == np[1])
					{
						if (proc2s.TryGetValue("TDeff", out value))
							np[1] = Convert.ToInt32(value);
					}
					if (proc2s.TryGetValue("OFFSET", out value))
						of[1] = Convert.ToSingle(value);
					if (proc2s.TryGetValue("SF", out value))
						sf[1] = Convert.ToSingle(value);
					if (proc2s.TryGetValue("SW_p", out value))
						sw[1] = Convert.ToSingle(value);
				}
			} 
			else
				np[1] = 1;

			// WRITE THE FILE TO THE SOCKET
			if (commentsamp.Length > 0)
				WriteLine(stream, commentsamp);
			if (commentcom.Length > 0)
				WriteLine(stream, commentcom);
			if (pulprog.Length > 0)
			{
				if (nuc1.Length > 0)
				{
					pulprog = pulprog + " - " + nuc1;
					if (nuc2.Length>0 && 0!=nuc2.CompareTo("off"))
						pulprog = pulprog + "{" + nuc2 + "}";
				}
				WriteLine(stream, pulprog);
			}
			WriteLine(stream, "NC_proc=" + ncproc.ToString());
            if (commentdataclass.Length > 0)
                WriteLine(stream, "dataclass=" + commentdataclass);
			WriteLine(stream, ".");

			SaneHD hd = new SaneHD(36);
			int dim = 1;
			if (Is2D)
				dim=2;
			hd.Insert(dim, 0);
			for (i=0; i<dim; i++) {
				hd.Insert(np[i], 4*(1+i*4));
				hd.Insert(sf[i], 4*(2+i*4));
				hd.Insert(sw[i], 4*(3+i*4));
				hd.Insert(of[i], 4*(4+i*4));
			}
			hd.Write(stream);
			if (1 == np[1])
				file = reqfilename + @"\1r";
			else
				file = reqfilename + @"\2rr";
			FileStream fs = new FileStream(file, FileMode.Open, FileAccess.Read);
			BinaryReader br = new BinaryReader(fs);
			SaneHD dt = new SaneHD(4*np[0]);

			// DIVIDE EACH DATA VALUE BY normcon IF NC_proc < 0
			int normcon = 1;
			while (ncproc < 0) 
			{
				normcon *= 2;
				ncproc++;
			}
			if (bytord == 0)
			{
				for (i=0; i<np[1]; i++)
				{
					for (int j=0; j<np[0]; j++) 
						dt.Insert(br.ReadInt32()/normcon, 4*j);
					dt.Write(stream);
				}
			}
			else
			{
				for (i=0; i<np[1]; i++)
				{
					for (int j=0; j<np[0]; j++) 
						dt.Insert(IPAddress.NetworkToHostOrder(br.ReadInt32())/normcon, 4*j);
					dt.Write(stream);
				}
			}
			br.Close();
		}
	}	// class NMRThread
}		// namespace TestServer
