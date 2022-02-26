// Flutter packages
import 'package:flutter/material.dart';
import 'package:font_awesome_flutter/font_awesome_flutter.dart';
import 'package:snoozeless/theme.dart';

// Custom packages
import 'package:snoozeless/services/firestore.dart';
import 'package:snoozeless/services/models.dart';

class DeviceEditScreen extends StatelessWidget {
  final String deviceId;

  const DeviceEditScreen({Key? key, required this.deviceId}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Edit Device'),
      ),
      body: DeviceEditForm(deviceId: deviceId),
    );
  }
}

class DeviceEditForm extends StatefulWidget {
  final String deviceId;

  const DeviceEditForm({Key? key, required this.deviceId}) : super(key: key);

  @override
  _DeviceEditFormState createState() => _DeviceEditFormState();
}

class _DeviceEditFormState extends State<DeviceEditForm> {
  final GlobalKey<FormState> _formKey = GlobalKey<FormState>();

  @override
  Widget build(BuildContext context) {
    const Map timeZonesMap = {
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

    return FutureBuilder(
      future: FirestoreService().getDevice(widget.deviceId),
      builder: (context, snapshot) {
        if (snapshot.connectionState == ConnectionState.waiting) {
          return const Text('Loading');
        } else if (snapshot.hasError) {
          return const Text('Error in edit_device.dart');
        } else if (snapshot.hasData) {
          var serverDevice = snapshot.data! as Device;
          var localDevice = Device();

          return Form(
            key: _formKey,
            child: Container(
              padding: const EdgeInsets.only(
                left: 20,
                right: 20,
                top: 25,
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  const Text('Device Name'),
                  Container(
                    padding: const EdgeInsets.only(bottom: 45),
                    child: TextFormField(
                      initialValue: serverDevice.deviceName,
                      validator: (value) {
                        if (value == null || value.isEmpty) {
                          return 'Device Name cannot be blank.';
                        } else {
                          localDevice.deviceName = value;
                          return null;
                        }
                      },
                    ),
                  ),
                  const Text('Time Zone'),
                  Container(
                    padding: const EdgeInsets.only(bottom: 45),
                    child: DropdownButtonFormField(
                      value: timeZonesMap.keys.firstWhere(
                          (key) =>
                              timeZonesMap[key] ==
                              serverDevice.timeZoneAdjustment,
                          orElse: () => null),
                      items: timeZonesMap.keys
                          .map((key) => key.toString())
                          .toList()
                          .map<DropdownMenuItem<String>>((String value) {
                        return DropdownMenuItem<String>(
                          value: value,
                          child: Text(value),
                        );
                      }).toList(),
                      validator: (value) {
                        localDevice.timeZoneAdjustment = timeZonesMap[value];
                      },
                      onChanged: (value) {},
                    ),
                  ),
                  // TODO: Write form of this type of button?
                  Row(
                    children: [
                      Flexible(
                        fit: FlexFit.tight,
                        child: Container(
                          padding: const EdgeInsets.only(right: 5),
                          height: 50,
                          // TODO: Change this to an Outlined Button at this level.
                          child: ElevatedButton.icon(
                            icon: const Icon(FontAwesomeIcons.check),
                            label: const Text('Save'),
                            style: OutlinedButton.styleFrom(
                              primary: Colors.green,
                              backgroundColor: appTheme.canvasColor,
                              elevation: 0,
                              side: const BorderSide(
                                  width: 2.0, color: Colors.green),
                              shape: RoundedRectangleBorder(
                                borderRadius: BorderRadius.circular(5.0),
                              ),
                            ),
                            onPressed: () {
                              if (_formKey.currentState!.validate()) {
                                FirestoreService().updateDevice(
                                  serverDevice,
                                  localDevice,
                                );
                                Navigator.of(context).pop();
                              }
                            },
                          ),
                        ),
                      ),
                      Flexible(
                        fit: FlexFit.tight,
                        child: Container(
                          padding: const EdgeInsets.only(left: 5),
                          height: 50,
                          // TODO: Change this to an Outlined Button at this level.
                          child: ElevatedButton.icon(
                            icon: const Icon(FontAwesomeIcons.trash),
                            label: const Text('Delete'),
                            style: OutlinedButton.styleFrom(
                              primary: Colors.red,
                              backgroundColor: appTheme.canvasColor,
                              elevation: 0,
                              side: const BorderSide(
                                  width: 2.0, color: Colors.red),
                              shape: RoundedRectangleBorder(
                                borderRadius: BorderRadius.circular(5.0),
                              ),
                            ),
                            // TODO: Add method to delete device.
                            onPressed: () {},
                          ),
                        ),
                      ),
                    ],
                  ),
                ],
              ),
            ),
          );
        } else {
          return const Text('No data found in database.');
        }
      },
    );
  }
}
