// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/services/firestore.dart';
import 'package:snoozeless/services/models.dart';
import 'package:snoozeless/shared/time_zones_map.dart';
import 'package:snoozeless/shared/edit_device_bottom_button.dart';

// Third party packages
import 'package:font_awesome_flutter/font_awesome_flutter.dart';

class EditDeviceForm extends StatefulWidget {
  final String deviceId;

  const EditDeviceForm({Key? key, required this.deviceId}) : super(key: key);

  @override
  _EditDeviceFormState createState() => _EditDeviceFormState();
}

class _EditDeviceFormState extends State<EditDeviceForm> {
  final GlobalKey<FormState> _formKey = GlobalKey<FormState>();

  @override
  Widget build(BuildContext context) {
    return FutureBuilder(
      future: FirestoreService().getDevice(widget.deviceId),
      builder: (context, snapshot) {
        if (snapshot.connectionState == ConnectionState.waiting) {
          return const Text('Loading');
        } else if (snapshot.hasError) {
          return const Text('Error in edit_device_form.dart');
        } else if (snapshot.hasData) {
          var initialDevice = snapshot.data! as Device;
          var modifiedDevice = Device();

          return Padding(
            padding: const EdgeInsets.only(
              left: 20,
              right: 20,
              top: 25,
            ),
            child: Form(
              key: _formKey,
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  const Text('Device Name'),
                  Container(
                    padding: const EdgeInsets.only(bottom: 45),
                    child: TextFormField(
                      initialValue: initialDevice.deviceName,
                      validator: (value) {
                        if (value == null || value.isEmpty) {
                          return 'Device Name cannot be blank.';
                        } else {
                          modifiedDevice.deviceName = value;
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
                              initialDevice.timeZoneAdjustment,
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
                        modifiedDevice.timeZoneAdjustment = timeZonesMap[value];
                      },
                      onChanged: (value) {},
                    ),
                  ),
                  Row(
                    children: [
                      EditDeviceBottomButton(
                        color: Colors.green,
                        icon: FontAwesomeIcons.check,
                        text: 'Save',
                        method: () {
                          if (_formKey.currentState!.validate()) {
                            FirestoreService().updateDevice(
                              initialDevice,
                              modifiedDevice,
                            );
                            Navigator.of(context).pop();
                          }
                        },
                      ),
                      const SizedBox(
                        width: 10,
                      ),
                      EditDeviceBottomButton(
                        color: Colors.red,
                        icon: FontAwesomeIcons.trash,
                        text: 'Delete',
                        method: () {
                          showDialog(
                            context: context,
                            builder: (_) => DeleteDeviceDialog(
                              serverDevice: initialDevice,
                            ),
                          );
                        },
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

class DeleteDeviceDialog extends StatelessWidget {
  final Device serverDevice;
  const DeleteDeviceDialog({
    Key? key,
    required this.serverDevice,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return AlertDialog(
        title: Text('Delete ' + serverDevice.deviceName + '?'),
        // TODO: Improve this sentence.
        content: const Text(
            'This device and its alarms will be permanently deleted. You will need to set up the device again to use it in the future.'),
        actions: [
          TextButton(
            child: const Text('Delete'),
            style: TextButton.styleFrom(
              padding: const EdgeInsets.only(
                right: 30,
              ),
              textStyle: const TextStyle(
                fontSize: 18,
              ),
            ),
            onPressed: () async {
              // TODO: Something in this function is throwing an error
              await FirestoreService().deleteDevice(serverDevice.deviceId);
              // TODO: Replace this method with one that uses .popUntil('namedRoute')
              int count = 0;
              Navigator.of(context).popUntil(
                (_) => count++ >= 3,
              );
            },
          ),
          TextButton(
            child: const Text('Cancel'),
            style: TextButton.styleFrom(
              padding: const EdgeInsets.only(
                right: 25,
              ),
              textStyle: const TextStyle(
                fontSize: 18,
              ),
            ),
            onPressed: () {
              Navigator.of(context).pop();
            },
          ),
        ]);
  }
}
