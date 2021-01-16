using System;
using System.Net;
using System.Net.Sockets;

namespace nmrServerThread
{
	/// <summary>
	/// Summary description for SaneHD.
	/// </summary>
	public class SaneHD
	{
		byte[] buffer;

		public SaneHD(int size)
		{
			buffer = new byte[size];
		}

		public void InsertChar(char c, int offset)
		{
			buffer[offset] = (byte)c;
		}

		public void Write(NetworkStream stream)
		{
			stream.Write(buffer, 0, buffer.Length);
		}

		public void Insert(object obj, int offset)
		{
			byte[] buf;
			if (obj.GetType() == Type.GetType("System.Double"))
			{
				buf = BitConverter.GetBytes((Double)obj);
			}
			else if (obj.GetType() == Type.GetType("System.Single"))
			{
				buf = BitConverter.GetBytes((Single)obj);
			}
			else if (obj.GetType() == Type.GetType("System.Int32"))
			{
				buf = BitConverter.GetBytes((Int32)obj);
			}
			else if (obj.GetType() == Type.GetType("System.String"))
			{
				String str = (String)obj;
				if (str.Length < 8) 
				{
					for (int i=0; i<str.Length; i++)
						buffer[i+offset] = (byte)str[i];
				}
				return;
			}
			else
			{
				throw new ArgumentException("Must be Int32, Single or Double type.");
			}
			if (buf.Length == 8) 
			{
				Int64 i = BitConverter.ToInt64(buf, 0);
				i = IPAddress.HostToNetworkOrder(i);
				buf = BitConverter.GetBytes(i);
			}
			else
			{
				Int32 i = BitConverter.ToInt32(buf, 0);
				i = IPAddress.HostToNetworkOrder(i);
				buf = BitConverter.GetBytes(i);
			}
			for (int i=0; i<buf.Length; i++)
				buffer[i+offset] = buf[i];
		}
	}
}
