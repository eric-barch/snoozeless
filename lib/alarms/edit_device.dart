import 'package:flutter/material.dart';
import 'package:font_awesome_flutter/font_awesome_flutter.dart';
import 'package:snoozeless/services/firestore.dart';
import 'package:snoozeless/services/models.dart';

class DeviceEditScreen extends StatelessWidget {
  final String deviceId;
  const DeviceEditScreen({Key? key, required this.deviceId}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return StreamBuilder<Device>(
      stream: FirestoreService().streamDevice(deviceId),
      builder: (context, snapshot) {
        if (snapshot.connectionState == ConnectionState.waiting) {
          return const Text('Loading');
        } else if (snapshot.hasError) {
          return const Text('Error in editDevice.dart');
        } else if (snapshot.hasData) {
          var device = snapshot.data!;

          return Scaffold(
            appBar: AppBar(
              title: const Text('Edit Device'),
              actions: [
                IconButton(
                  icon: const Icon(
                    FontAwesomeIcons.check,
                    size: 20,
                  ),
                  alignment: Alignment.center,
                  onPressed: () {
                    FirestoreService().updateDevice(device);
                    Navigator.of(context).pop();
                  },
                ),
              ],
            ),
            body: Padding(
              padding: const EdgeInsets.all(20),
              child: Column(
                // mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  const Text('Device Name'),
                  Padding(
                    padding: const EdgeInsets.only(top: 10, bottom: 20),
                    child: TextField(
                      controller: TextEditingController(),
                      onChanged: (value) {
                        device.deviceName = value;
                      },
                    ),
                  ),
                  const Text('Time Zone'),
                  const Padding(
                    padding: EdgeInsets.only(top: 10, bottom: 20),
                    child: TimeZoneDropDown(),
                  ),
                  Padding(
                    padding: const EdgeInsets.only(bottom: 20),
                    child: ElevatedButton.icon(
                      icon: const Icon(
                        FontAwesomeIcons.times,
                        color: Colors.white,
                        size: 20,
                      ),
                      style: TextButton.styleFrom(
                        padding: const EdgeInsets.all(20),
                        backgroundColor: Colors.red,
                      ),
                      onPressed: () {},
                      label: Text(
                        device.deviceId == '' ? 'Cancel' : 'Delete Device',
                      ),
                    ),
                  ),
                ],
              ),
            ),
          );
        } else {
          return const Text('No alarm found. Check database.');
        }
      },
    );
  }
}

class TimeZoneDropDown extends StatefulWidget {
  const TimeZoneDropDown({Key? key}) : super(key: key);

  @override
  State<TimeZoneDropDown> createState() => _TimeZoneDropDownState();
}

class _TimeZoneDropDownState extends State<TimeZoneDropDown> {
  static Map timeZonesMap = {
    'UTC-12:00 (U.S. Minor Outlying Islands)': -12,
    'UTC-11:00 (Niue)': -11,
    'UTC-10:00 (Honolulu)': -10,
    'UTC-09:30 (French Polynesia)': -9.5,
    'UTC-09:00 (Anchorage)': -9,
    'UTC-08:00 (Los Angeles)': -8,
    'UTC-07:00 (Denver)': -7,
    'UTC-06:00 (Mexico City)': -6,
    'UTC-05:00 (New York)': -5,
    'UTC-04:00 (Santiago)': -4,
    'UTC-03:30 (St. John\'s)': -3.5,
    'UTC-03:00 (Buenos Aires)': -3,
    'UTC-02:00 (Fernando de Noronha)': -2,
    'UTC-01:00 (Cape Verde)': -1,
    'UTC+00:00 (London)': 0,
    'UTC+01:00 (Berlin)': 1,
    'UTC+02:00 (Cairo)': 2,
    'UTC+03:00 (Moscow)': 3,
    'UTC+03:30 (Tehran)': 3.5,
    'UTC+04:00 (Dubai)': 4,
    'UTC+04:30 (Kabul)': 4.5,
    'UTC+05:00 (Karachi)': 5,
    'UTC+05:30 (Mumbai)': 5.5,
    'UTC+05:45 (Kathmandu)': 5.75,
    'UTC+06:00 (Dhaka)': 6,
    'UTC+06:30 (Yangon)': 6.5,
    'UTC+07:00 (Jakarta)': 7,
    'UTC+08:00 (Shanghai)': 8,
    'UTC+08:45 (Eucla)': 8.75,
    'UTC+09:00 (Tokyo)': 9,
    'UTC+09:30 (Adelaide)': 9.5,
    'UTC+10:00 (Sydney)': 10,
    'UTC+10:30 (Lord Howe Island)': 10.5,
    'UTC+11:00 (Nouméa)': 11,
    'UTC+12:00 (Auckland)': 12,
    'UTC+12:34 (Chatham Islands)': 12.75,
    'UTC+13:00 (Phoenix Islands)': 13,
    'UTC+14:00 (Line Islands)': 14,
  };

  String dropdownValue = 'UTC-05:00 (New York)';

  @override
  Widget build(BuildContext context) {
    return DropdownButton<String>(
      value: dropdownValue,
      isExpanded: true,
      icon: const Icon(FontAwesomeIcons.chevronDown),
      onChanged: (String? newValue) {
        setState(() {
          dropdownValue = newValue!;
        });
      },
      items: timeZonesMap.keys
          .map((key) => key.toString())
          .toList()
          .map<DropdownMenuItem<String>>((String value) {
        return DropdownMenuItem<String>(
          value: value,
          child: Text(value),
        );
      }).toList(),
    );
  }
}
