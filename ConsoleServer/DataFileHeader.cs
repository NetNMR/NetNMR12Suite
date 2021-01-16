using System;
using System.IO;
using System.Net;
using System.Net.Sockets;

namespace nmrServerThread
{
	struct DataFileHeader
	{
		public Int32 nblocks, ntraces, np, ebytes, tbytes, bbytes, nbheaders;
		public Int16 vers_id, status;
		public void Read(BinaryReader br)
		{
			byte[] buffer = new byte[32];
			br.Read(buffer, 0, 32);
			nblocks   = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(buffer,  0));
			ntraces   = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(buffer,  4));
			np        = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(buffer,  8));
			ebytes    = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(buffer, 12));
			tbytes    = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(buffer, 16));
			bbytes    = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(buffer, 20));
			vers_id   = IPAddress.NetworkToHostOrder(BitConverter.ToInt16(buffer, 24));
			status    = IPAddress.NetworkToHostOrder(BitConverter.ToInt16(buffer, 26));
			nbheaders = IPAddress.NetworkToHostOrder(BitConverter.ToInt32(buffer, 28));
		}
	}
}