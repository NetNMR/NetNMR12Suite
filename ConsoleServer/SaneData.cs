using System;
using System.Net;
using System.Net.Sockets;

namespace nmrServerThread
{
	/// <summary>
	/// Summary description for SaneData.
	/// </summary>
	public class SaneData
	{
		byte[] buffer;

		public SaneData(int size)
		{
			buffer = new byte[4*size];
		}

		public void Insert(float val, int offset)
		{
			byte[] bval = BitConverter.GetBytes(val);
			for (int i=0; i<4; i++)
				buffer[i+4*offset] = bval[3-i];
		}

		public void Write(NetworkStream stream)
		{
			stream.Write(buffer, 0, buffer.Length);
		}

		public void Copy(byte[] bdata)
		{
			bdata.CopyTo(buffer, 0);
		}
	}
}
