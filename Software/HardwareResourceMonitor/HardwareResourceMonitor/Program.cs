using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using OpenHardwareMonitor.Hardware;
using System.IO.Ports;

namespace HardwareResourceMonitor
{
    class Program
    {
        static SerialPort _serialPort;
        public class UpdateVisitor : IVisitor
        {
            public void VisitComputer(IComputer computer)
            {
                computer.Traverse(this);
            }
            public void VisitHardware(IHardware hardware)
            {
                hardware.Update();
                foreach (IHardware subHardware in hardware.SubHardware) subHardware.Accept(this);
            }
            public void VisitSensor(ISensor sensor) { }
            public void VisitParameter(IParameter parameter) { }
        }
        static void GetSystemInfo()
        {
            UpdateVisitor updateVisitor = new UpdateVisitor();
            Computer computer = new Computer();
            computer.Open();
            computer.CPUEnabled = true;
            computer.GPUEnabled = true;
            computer.RAMEnabled = true;
            computer.Accept(updateVisitor);
            string CPUInfoString = "";
            string GPUInfoString = "";
            string RAMInfoString = "";
            string SystemInfoString = "";
            for (int i = 0; i < computer.Hardware.Length; i++)
            {
                if (computer.Hardware[i].HardwareType == HardwareType.CPU)
                {
                    CPUInfoString = "";
                    for (int j = 0; j < computer.Hardware[i].Sensors.Length; j++)
                    {
                        if (computer.Hardware[i].Sensors[j].SensorType == SensorType.Temperature &&
                            computer.Hardware[i].Sensors[j].Name == "CPU Package")
                        {
                            CPUInfoString += computer.Hardware[i].Sensors[j].Name + ":" + computer.Hardware[i].Sensors[j].Value.ToString() ;
                            // Console.WriteLine(CPUInfoString);
                            //_serialPort.WriteLine(computer.Hardware[i].Sensors[j].Name + ":" + computer.Hardware[i].Sensors[j].Value.ToString() );
                        }
                    }
                }
                if(computer.Hardware[i].HardwareType == HardwareType.GpuNvidia)
                {
                    GPUInfoString = "";
                    for (int j = 0; j < computer.Hardware[i].Sensors.Length; j++)
                    {
                        if (computer.Hardware[i].Sensors[j].SensorType == SensorType.Temperature &&
                            computer.Hardware[i].Sensors[j].Name == "GPU Core")
                        {
                            GPUInfoString += computer.Hardware[i].Sensors[j].Name + ":" + computer.Hardware[i].Sensors[j].Value.ToString();
                        }
                    }
                }
                if (computer.Hardware[i].HardwareType == HardwareType.RAM)
                {
                    RAMInfoString = "";
                    for (int j = 0; j < computer.Hardware[i].Sensors.Length; j++)
                    {
                        if (computer.Hardware[i].Sensors[j].SensorType == SensorType.Data &&
                            computer.Hardware[i].Sensors[j].Name == "Used Memory")
                        {
                            RAMInfoString += computer.Hardware[i].Sensors[j].Name + ":" + computer.Hardware[i].Sensors[j].Value.ToString();
                        }
                    }
                }
            }
            SystemInfoString = CPUInfoString + "," + GPUInfoString + "," + RAMInfoString;
            Console.WriteLine(SystemInfoString);
            _serialPort.WriteLine(SystemInfoString);
            computer.Close();
        }
        static void Main(string[] args)
        {
            //Temp Choose COM7
            _serialPort = new SerialPort();
            _serialPort.PortName = "COM7";
            _serialPort.BaudRate = 115200;
            _serialPort.DataBits = 8;
            _serialPort.Parity = Parity.None;
            _serialPort.StopBits = StopBits.One;
            _serialPort.Open();
            while (true)
            {
                GetSystemInfo();
                Thread.Sleep(1000);
            }
            _serialPort.Close();
        }
    }
}
