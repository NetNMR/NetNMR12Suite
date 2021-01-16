using System;
using System.Collections.Generic;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.Reflection;
using System.Diagnostics;

namespace ConsoleServer
{
    class Program
    {
        static void Main(string[] args)
        {
            TcpListener tcp = new TcpListener(IPAddress.Any, 1666);
            tcp.Start();
            Module[] moduleArray;
            moduleArray = Assembly.GetExecutingAssembly().GetModules(false);

            // In a simple project with only one module, the module at index
            // 0 will be the module containing this class.
            Module myModule = moduleArray[0];
            Console.Title = "ConsoleServer";
            FileVersionInfo myInfo = FileVersionInfo.GetVersionInfo(myModule.FullyQualifiedName);
            string[] ver = myInfo.FileVersion.ToString().Split(new char[] { '.' });
            string msg = String.Concat("Version ", String.Join(".", ver, 0, 3));
            Console.WriteLine(msg);
            Console.WriteLine("Listening on port 1666.");
            int Connection = 0;
            while (true)
            {
                Socket sock = tcp.AcceptSocket();
                //sock.SetSocketOption(SocketOptionLevel.Tcp, SocketOptionName.NoDelay, true);
                if (sock.Connected)
                {
                    Connection++;
                    msg = "Connection " + Connection.ToString() + " from: " + ((IPEndPoint)sock.RemoteEndPoint).ToString();
                    Console.WriteLine(msg);
                    byte[] greet = Encoding.ASCII.GetBytes("Tefid Server\n");
                    sock.Send(greet);
                    nmrServerThread.NMRThread server = new nmrServerThread.NMRThread(sock, Connection);
                    Thread thr = new Thread(new ThreadStart(server.Serve));
                    thr.Start();
                }
            }
        }
    }
}
